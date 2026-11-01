#!/bin/bash
# Runs IVFFlat for every D of the assignment (1000, 5000, 10000, 25000) and
# for a list of nprobe values, and prints one summary table.
#
# Usage: ./run_ivfflat.sh [set] [nprobe list] [kclusters] [vocab] [S] [seed]
#   set:         validation, test or both        (default: validation)
#   nprobe list: values in quotes, e.g. "1 5 10" (default: "1 5 10 20")
#   kclusters:   0 means ceil(sqrt(n))           (default: 0)
#   vocab, S, seed                                (default: 256 500 1)
#
# Example: ./run_ivfflat.sh test "10" 200
#
# Output files: results/ivfflat/<set>_D<D>_k<kclusters>_np<nprobe>.txt  (format of the assignment)
#               results/ivfflat/<set>_D<D>_k<kclusters>_np<nprobe>.log  (everything printed)
#               results/ivfflat/summary.txt                             (the table below)
# k0 in a file name means kclusters = ceil(sqrt(n)).

# Work from the folder of this script (code/), so it can be started from
# anywhere. Paths given as arguments are relative to code/.
cd "$(dirname "$0")" || exit 1

SET=${1:-validation}
NPROBES=${2:-"1 5 10 20"}
KCLUSTERS=${3:-0}
VOCAB=${4:-256}
S=${5:-500}
SEED=${6:-1}

if [ "$SET" = "both" ]; then
    SETS="validation test"
elif [ "$SET" = "validation" ] || [ "$SET" = "test" ]; then
    SETS="$SET"
else
    echo "Error: set must be validation, test or both"
    exit 1
fi

if [ ! -x ./search ]; then
    echo "Error: ./search not found, run make first"
    exit 1
fi

OUT_DIR=results/ivfflat
mkdir -p "$OUT_DIR"
SUMMARY="$OUT_DIR/summary.txt"

{
    echo "IVFFlat, K = $VOCAB, S = $S, seed = $SEED"
    printf "%-10s %6s %7s %9s %6s | %9s %9s %8s %9s | %9s | %8s %8s\n" \
        "set" "D" "db size" "kclusters" "nprobe" "Recall@5" "Recall@10" "mAP@10" "time (ms)" \
        "compared" "mAP illu" "mAP view"
} > "$SUMMARY"

for set in $SETS; do
    for D in 1000 5000 10000 25000; do
        for nprobe in $NPROBES; do
            name="${set}_D${D}_k${KCLUSTERS}_np${nprobe}"
            echo "Running $name ..."
            log="$OUT_DIR/$name.log"
            ./search -set "$set" -D "$D" -vocab "$VOCAB" -S "$S" -seed "$SEED" \
                     -o "$OUT_DIR/$name.txt" -ivfflat -kclusters "$KCLUSTERS" -nprobe "$nprobe" \
                     > "$log" 2>&1
            if [ $? -ne 0 ]; then
                echo "Error: run $name failed, see $log"
                exit 1
            fi

            # values printed by the program
            dbSize=$(sed -n 's/.*database: \([0-9]*\) images.*/\1/p' "$log")
            clusters=$(sed -n 's/.*kclusters \([0-9]*\), nprobe.*/\1/p' "$log")
            read -r _ _ r5 r10 map time _ <<< "$(grep -E '^  all ' "$log")"
            mapIllu=$(grep -E '^  illumination ' "$log" | awk '{print $5}')
            mapView=$(grep -E '^  viewpoint ' "$log" | awk '{print $5}')
            compared=$(sed -n 's/.*(\([0-9.]*%\)).*/\1/p' "$log")

            printf "%-10s %6s %7s %9s %6s | %9s %9s %8s %9s | %9s | %8s %8s\n" \
                "$set" "$D" "$dbSize" "$clusters" "$nprobe" "$r5" "$r10" "$map" "$time" \
                "$compared" "$mapIllu" "$mapView" >> "$SUMMARY"
        done
    done
done

echo
cat "$SUMMARY"
