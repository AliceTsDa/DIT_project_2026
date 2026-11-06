#!/bin/bash
# Experiments of the assignment (step 10), following the protocol of the spec:
#   1. validation: run a parameter grid for every method and every D
#   2. selection:  for every method and D pick the fastest setting whose
#                  mAP@10 is at least TARGET (95%) of Exact Search's mAP@10;
#                  if no setting reaches it, the one with the best mAP@10
#   3. test:       run every method once per D with its chosen parameters
#                  (repeated RUNS times; quality is the same every time, the
#                  query time is averaged)
#
# Methods: exact, ivfflat, lsh, hypercube (IVFPQ is not implemented).
#
# Usage: ./run_experiments.sh [stage]
#   stage: all (default), validation, select, test
#
# Output (results/experiments/):
#   validation.csv        one line per validation run
#   chosen.csv            chosen parameters per method and D
#   test.csv              one line per test run (averaged time)
#   summary.md            tables of the chosen parameters and test results
#   validation/<method>/*.txt, test/<method>/*.txt   output files of every run

# Work from the folder of this script (code/), so it can be started from
# anywhere.
cd "$(dirname "$0")" || exit 1

STAGE=${1:-all}

VOCAB=256
S=500
SEED=1
DS=${DS:-"1000 5000 10000 25000"}   # can be overridden for a quick try
TARGET=0.95
RUNS=3

OUT=${OUT:-results/experiments}
COMMON="-vocab $VOCAB -S $S -seed $SEED"

if [ ! -x ./search ]; then
    echo "Error: ./search not found, run make first"
    exit 1
fi
mkdir -p "$OUT"

HEADER="set,method,D,params,recall5,recall10,map10,time_ms,compared,fewer10,map_illum,map_view"

# Runs one setting and prints one CSV line.
#   run_one <set> <D> <method> <params as one string> <output file>
run_one() {
    local set=$1 D=$2 method=$3 params=$4 file=$5
    local log
    log=$(./search -set "$set" -D "$D" $COMMON -o "$file" -$method $params 2>/dev/null)
    if [ $? -ne 0 ]; then
        echo "Error: run failed: -set $set -D $D -$method $params" >&2
        exit 1
    fi
    local r5 r10 map time fewer
    read -r _ _ r5 r10 map time fewer <<< "$(echo "$log" | grep -E '^  all ')"
    local mapIllum mapView compared
    mapIllum=$(echo "$log" | grep -E '^  illumination ' | awk '{print $5}')
    mapView=$(echo "$log" | grep -E '^  viewpoint ' | awk '{print $5}')
    compared=$(echo "$log" | sed -n 's/.*(\([0-9.]*\)%).*/\1/p')
    echo "$set,$method,$D,$params,$r5,$r10,$map,$time,$compared,$fewer,$mapIllum,$mapView"
}

# The parameter grid of a method for a given D, one setting per line
grid() {
    local method=$1 D=$2
    local n=$((115 + D))
    case $method in
        exact)
            echo ""
            ;;
        ivfflat)
            # kclusters: 0 = ceil(sqrt(n)), and twice as many
            local sq
            sq=$(awk -v n="$n" 'BEGIN { s = sqrt(n); print (s == int(s)) ? s : int(s) + 1 }')
            for kc in 0 $((2 * sq)); do
                for np in 1 2 5 10 20; do
                    echo "-kclusters $kc -nprobe $np"
                done
            done
            ;;
        lsh)
            for k in 4 6 8; do
                for L in 5 10 15; do
                    for w in 0.1 0.15 0.2 0.3; do
                        echo "-k $k -L $L -w $w"
                    done
                done
            done
            ;;
        hypercube)
            # M scales with the database size: 10%, 20%, 40% of n
            for kp in 8 10 12 14; do
                for w in 0.2 0.3 0.4; do
                    for M in $((n / 10)) $((n / 5)) $((2 * n / 5)); do
                        for pr in 50 100 500; do
                            echo "-kproj $kp -w $w -M $M -probes $pr"
                        done
                    done
                done
            done
            ;;
    esac
}

# A short file name for a setting, e.g. "-k 4 -L 5 -w 0.1" -> "k4_L5_w0.1"
setting_name() {
    if [ -z "$1" ]; then
        echo "default"
    else
        echo "$1" | sed 's/^-//; s/ -/_/g; s/ //g'
    fi
}

run_validation() {
    echo "$HEADER" > "$OUT/validation.csv"
    for method in exact ivfflat lsh hypercube; do
        mkdir -p "$OUT/validation/$method"
        for D in $DS; do
            echo "validation: $method D=$D"
            while read -r params; do
                file="$OUT/validation/$method/D${D}_$(setting_name "$params").txt"
                run_one validation "$D" "$method" "$params" "$file" >> "$OUT/validation.csv"
            done <<< "$(grid "$method" "$D")"
        done
    done
}

# chosen.csv: method,D,params,validation mAP,validation time,exact mAP
run_select() {
    awk -F, -v target="$TARGET" '
        NR == 1 { next }
        $2 == "exact" { exactMap[$3] = $7; exactTime[$3] = $8 }
        {
            key = $2 "," $3
            n[key]++
            params[key, n[key]] = $4; map[key, n[key]] = $7; time[key, n[key]] = $8
            methods[$2] = 1; ds[$3] = 1
        }
        END {
            print "method,D,params,val_map10,val_time_ms,exact_map10,rule"
            for (key in n) {
                split(key, part, ",")
                limit = target * exactMap[part[2]]
                # fastest setting that reaches the target
                best = 0
                for (i = 1; i <= n[key]; i++) {
                    if (map[key, i] >= limit && (best == 0 || time[key, i] < time[key, best])) {
                        best = i
                    }
                }
                rule = "fastest >= " target " x exact"
                if (best == 0) {
                    for (i = 1; i <= n[key]; i++) {
                        if (best == 0 || map[key, i] > map[key, best]) { best = i }
                    }
                    rule = "best mAP (target not reached)"
                }
                printf "%s,%s,%s,%s,%s,%s,%s\n", part[1], part[2], params[key, best],
                       map[key, best], time[key, best], exactMap[part[2]], rule
            }
        }' "$OUT/validation.csv" | { read -r header; echo "$header"; sort -t, -k1,1 -k2,2n; } > "$OUT/chosen.csv"
    echo "chosen parameters:"
    column -s, -t "$OUT/chosen.csv"
}

run_test() {
    echo "$HEADER" > "$OUT/test.csv"
    tail -n +2 "$OUT/chosen.csv" | while IFS=, read -r method D params _; do
        mkdir -p "$OUT/test/$method"
        file="$OUT/test/$method/D${D}_$(setting_name "$params").txt"
        echo "test: $method D=$D $params"
        # first run gives the quality, all runs give the time
        sum=0
        for ((r = 1; r <= RUNS; r++)); do
            line=$(run_one test "$D" "$method" "$params" "$file")
            [ $r -eq 1 ] && first="$line"
            time=$(echo "$line" | cut -d, -f8)
            sum=$(awk -v a="$sum" -v b="$time" 'BEGIN { print a + b }')
        done
        avg=$(awk -v s="$sum" -v r="$RUNS" 'BEGIN { printf "%.4f", s / r }')
        echo "$first" | awk -F, -v OFS=, -v t="$avg" '{ $8 = t; print }' >> "$OUT/test.csv"
    done
    write_summary
}

write_summary() {
    {
        echo "# Experiments (K = $VOCAB, S = $S, seed = $SEED)"
        echo
        echo "Selection on validation: fastest setting with mAP@10 >= $TARGET x Exact"
        echo "(else best mAP@10). Test query times: average of $RUNS runs."
        echo
        echo "## Chosen parameters (validation)"
        echo
        echo "| method | D | parameters | val mAP@10 | val time (ms) | exact mAP@10 | rule |"
        echo "|---|---|---|---|---|---|---|"
        tail -n +2 "$OUT/chosen.csv" | awk -F, '{ printf "| %s | %s | %s | %s | %s | %s | %s |\n", $1, $2, ($3 == "" ? "-" : $3), $4, $5, $6, $7 }'
        echo
        echo "## Test results"
        echo
        echo "| method | D | Recall@5 | Recall@10 | mAP@10 | time (ms) | compared (%) | < 10 results | mAP illum. | mAP viewp. |"
        echo "|---|---|---|---|---|---|---|---|---|---|"
        tail -n +2 "$OUT/test.csv" | sort -t, -k3,3n -k2,2 | awk -F, '{ printf "| %s | %s | %s | %s | %s | %s | %s | %s | %s | %s |\n", $2, $3, $5, $6, $7, $8, $9, $10, $11, $12 }'
    } > "$OUT/summary.md"
    echo
    cat "$OUT/summary.md"
}

case $STAGE in
    validation) run_validation ;;
    select) run_select ;;
    test) run_test ;;
    all) run_validation; run_select; run_test ;;
    *) echo "Error: stage must be all, validation, select or test"; exit 1 ;;
esac
