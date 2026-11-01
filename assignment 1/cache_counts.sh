#!/bin/bash
# Prints the number of SIFT descriptors of every file in the cache.
#
# Usage: ./cache_counts.sh [cache_dir]     (default: cache)
#
# Output: one line per cache file   <descriptors> <file>
#         and the totals at the end.
#
# Each cache file starts with a 4-byte int: the number of descriptors
# (see DescriptorSet::save). To be fast, the script does NOT start a program
# per file: one "head" reads the first 4 bytes of all files at once and one
# "od" turns them into numbers. (It does not check if files are damaged;
# use check_cache.sh for that.)

# Work from the folder of this script (code/), so it can be started from
# anywhere. Paths given as arguments are relative to code/.
cd "$(dirname "$0")" || exit 1

CACHE_DIR=${1:-cache}

if [ ! -d "$CACHE_DIR" ]; then
    echo "Error: cache directory $CACHE_DIR not found"
    exit 1
fi

# All descriptor files (not vectors/ or vocab/), in natural order (im2 before im10)
mapfile -t files < <(find "$CACHE_DIR/hpatches" "$CACHE_DIR/mirflickr" -maxdepth 1 -name '*.bin' | sort -V)

if [ ${#files[@]} -eq 0 ]; then
    echo "No cache files in $CACHE_DIR"
    exit 0
fi

# First 4 bytes of every file -> one number per line, next to its file name
paste <(printf '%s\0' "${files[@]}" | xargs -0 head -q -c 4 | od -An -v -t d4 -w4) \
      <(printf '%s\n' "${files[@]}") |
awk '{
    printf "%7d %s\n", $1, $2
    total += $1
    if ($2 ~ /\/hpatches\//)  { hp += $1; hpFiles++ }
    if ($2 ~ /\/mirflickr\//) { mir += $1; mirFiles++ }
}
END {
    print "----------------------------------------"
    printf "HPatches:  %6d files %10d descriptors\n", hpFiles, hp
    printf "MIRFlickr: %6d files %10d descriptors\n", mirFiles, mir
    printf "Total:     %6d files %10d descriptors\n", NR, total
}'
