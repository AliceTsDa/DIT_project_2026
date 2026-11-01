#!/bin/bash
# Prints the details of every vocabulary (k-means centers) in the cache.
#
# Usage: ./vocab_info.sh [cache_dir]     (default: cache)
#
# Vocabulary file format (see Vocabulary::save):
#   int K, int dim, then K * dim floats (center 1, center 2, ...).
# A file is OK when its size is exactly 8 + K * dim * 4 bytes.
#
# For every file it prints:
#   - K, S and seed (from the file name vocab_K<K>_S<S>_seed<seed>.bin)
#   - K and dim stored in the file, size, date, OK / DAMAGED
#   - range and average of the center values (SIFT values are 0..255)
#   - length (Euclidean norm) of the centers: min, max, average

# Work from the folder of this script (code/), so it can be started from
# anywhere. Paths given as arguments are relative to code/.
cd "$(dirname "$0")" || exit 1

# Decimal point, not comma: with a Greek locale od prints "194,3" and awk
# would not read it as a number.
export LC_ALL=C

CACHE_DIR=${1:-cache}
VOCAB_DIR="$CACHE_DIR/vocab"

if [ ! -d "$VOCAB_DIR" ]; then
    echo "No vocabulary directory $VOCAB_DIR"
    exit 1
fi

shopt -s nullglob
files=("$VOCAB_DIR"/vocab_*.bin)
if [ ${#files[@]} -eq 0 ]; then
    echo "No vocabularies in $VOCAB_DIR"
    exit 0
fi

count=0
for file in $(printf '%s\n' "${files[@]}" | sort -V); do
    name=$(basename "$file")
    count=$((count + 1))

    # K, S and seed from the file name
    nameK=$(echo "$name" | sed -n 's/.*_K\([0-9]*\)_.*/\1/p')
    nameS=$(echo "$name" | sed -n 's/.*_S\([0-9]*\)_.*/\1/p')
    nameSeed=$(echo "$name" | sed -n 's/.*_seed\([0-9]*\)\.bin/\1/p')

    size=$(stat -c %s "$file")
    date=$(date -r "$file" "+%Y-%m-%d %H:%M")

    echo "========================================"
    echo "$name"
    echo "  from name:     K = $nameK, S = $nameS, seed = $nameSeed"
    echo "  file:          $size bytes, created $date"

    if [ "$size" -lt 8 ]; then
        echo "  status:        DAMAGED (file too small)"
        continue
    fi

    # header: two ints
    read -r fileK fileDim <<< "$(od -An -t d4 -N8 "$file")"
    echo "  in file:       K = $fileK, dim = $fileDim"

    expected=$((8 + fileK * fileDim * 4))
    if [ "$size" -ne "$expected" ]; then
        echo "  status:        DAMAGED (expected $expected bytes)"
        continue
    fi
    if [ "$fileK" != "$nameK" ]; then
        echo "  status:        DAMAGED (K in file differs from the name)"
        continue
    fi
    echo "  status:        OK"

    # all floats after the 8-byte header, one per line, then statistics
    od -An -v -t f4 -w4 -j 8 "$file" | awk -v dim="$fileDim" '
    {
        value = $1
        if (NR == 1 || value < minValue) minValue = value
        if (NR == 1 || value > maxValue) maxValue = value
        sumValue += value

        squares += value * value
        if (NR % dim == 0) {          # end of one center
            norm = sqrt(squares)
            centers++
            if (centers == 1 || norm < minNorm) minNorm = norm
            if (centers == 1 || norm > maxNorm) maxNorm = norm
            sumNorm += norm
            squares = 0
        }
    }
    END {
        printf "  values:        min %.2f, max %.2f, avg %.2f\n", minValue, maxValue, sumValue / NR
        printf "  center length: min %.1f, max %.1f, avg %.1f  (%d centers)\n",
               minNorm, maxNorm, sumNorm / centers, centers
    }'
done

echo "========================================"
echo "Vocabularies: $count"
