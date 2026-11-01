#!/bin/bash
# Runs Exact Search for every D of the assignment (1000, 5000, 10000, 25000)
# and prints one summary table.
#
# Usage: ./run_exact.sh [set] [vocab] [S] [seed]
#   set:   validation, test or both   (default: both)
#   vocab: K                          (default: 256)
#   S:     descriptors per image      (default: 500)
#   seed:                             (default: 1)
#
# Output files: results/exact/<set>_D<D>.txt   (format of the assignment)
#               results/exact/<set>_D<D>.log   (everything the program printed)
#               results/exact/summary.txt      (the table below)

# Work from the folder of this script (code/), so it can be started from
# anywhere. Paths given as arguments are relative to code/.
cd "$(dirname "$0")" || exit 1

SET=${1:-both}
VOCAB=${2:-256}
S=${3:-500}
SEED=${4:-1}

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

OUT_DIR=results/exact
mkdir -p "$OUT_DIR"
SUMMARY="$OUT_DIR/summary.txt"

{
    echo "Exact Search, K = $VOCAB, S = $S, seed = $SEED"
    printf "%-10s %6s %7s | %9s %9s %8s %9s | %8s %8s\n" \
        "set" "D" "db size" "Recall@5" "Recall@10" "mAP@10" "time (ms)" "mAP illu" "mAP view"
} > "$SUMMARY"

for set in $SETS; do
    for D in 1000 5000 10000 25000; do
        name="${set}_D${D}"
        echo "Running $name ..."
        ./search -set "$set" -D "$D" -vocab "$VOCAB" -S "$S" -seed "$SEED" \
                 -o "$OUT_DIR/$name.txt" -exact > "$OUT_DIR/$name.log" 2>&1
        if [ $? -ne 0 ]; then
            echo "Error: run $name failed, see $OUT_DIR/$name.log"
            exit 1
        fi

        # summary lines printed by the program:
        #   group  queries  Recall@5  Recall@10  mAP@10  time  <10
        log="$OUT_DIR/$name.log"
        dbSize=$(sed -n 's/.*database: \([0-9]*\) images.*/\1/p' "$log")
        read -r _ _ r5 r10 map time _ <<< "$(grep -E '^  all ' "$log")"
        mapIllu=$(grep -E '^  illumination ' "$log" | awk '{print $5}')
        mapView=$(grep -E '^  viewpoint ' "$log" | awk '{print $5}')

        printf "%-10s %6s %7s | %9s %9s %8s %9s | %8s %8s\n" \
            "$set" "$D" "$dbSize" "$r5" "$r10" "$map" "$time" "$mapIllu" "$mapView" >> "$SUMMARY"
    done
done

echo
cat "$SUMMARY"
