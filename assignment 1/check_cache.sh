#!/bin/bash
# Checks the SIFT descriptor cache: for every image, is its cache file there,
# is it complete, and how many descriptors does it have.
#
# Usage: ./check_cache.sh [cache_dir] [split_file] [num_mirflickr]
#   defaults:            cache       split.txt    25000
#
# Cache file format (see DescriptorSet::save):
#   4 bytes int count, then count * 128 bytes.
# A file is OK when its size is exactly 4 + count * 128.
#
# Output: one line per image   <status> <descriptors> <image>
#   status: OK, MISSING or DAMAGED
# and a summary at the end.

CACHE_DIR=${1:-cache}
SPLIT_FILE=${2:-split.txt}
NUM_MIR=${3:-25000}

if [ ! -d "$CACHE_DIR" ]; then
    echo "Error: cache directory $CACHE_DIR not found"
    exit 1
fi
if [ ! -f "$SPLIT_FILE" ]; then
    echo "Error: split file $SPLIT_FILE not found"
    exit 1
fi

ok=0
missing=0
damaged=0
empty=0
total_descriptors=0

# Checks one cache file and prints one line
check_file() {
    local file=$1
    local image=$2

    if [ ! -f "$file" ]; then
        printf "%-8s %6s %s\n" "MISSING" "-" "$image"
        missing=$((missing + 1))
        return
    fi

    local size
    size=$(stat -c %s "$file")
    if [ "$size" -lt 4 ]; then
        printf "%-8s %6s %s\n" "DAMAGED" "-" "$image"
        damaged=$((damaged + 1))
        return
    fi

    # first 4 bytes = number of descriptors
    local count
    count=$(od -An -t d4 -N4 "$file" | tr -d ' ')
    if [ "$size" -ne $((4 + count * 128)) ]; then
        printf "%-8s %6s %s\n" "DAMAGED" "$count" "$image"
        damaged=$((damaged + 1))
        return
    fi

    printf "%-8s %6d %s\n" "OK" "$count" "$image"
    ok=$((ok + 1))
    total_descriptors=$((total_descriptors + count))
    if [ "$count" -eq 0 ]; then
        empty=$((empty + 1))
    fi
}

# HPatches: 6 images of every sequence in the split file
while read -r sequence part; do
    [ -z "$sequence" ] && continue
    for n in 1 2 3 4 5 6; do
        check_file "$CACHE_DIR/hpatches/${sequence}_${n}.bin" "$sequence/$n ($part)"
    done
done < "$SPLIT_FILE"

# MIRFlickr: im1 ... im<NUM_MIR>
for ((n = 1; n <= NUM_MIR; n++)); do
    check_file "$CACHE_DIR/mirflickr/im${n}.bin" "im${n}.jpg"
done

echo "----------------------------------------"
echo "Images checked:          $((ok + missing + damaged))"
echo "OK:                      $ok"
echo "  of these with 0 SIFT:  $empty"
echo "MISSING:                 $missing"
echo "DAMAGED:                 $damaged"
echo "Total descriptors (OK):  $total_descriptors"
