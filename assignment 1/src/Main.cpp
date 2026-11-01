// Image search with vector representations (assignment 1).
// Step 3: read the parameters and the split, get the SIFT descriptors
// of every image the run needs (cached) and build the vocabulary (cached).

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#include "DescriptorCache.h"
#include "DescriptorSet.h"
#include "GroupStats.h"
#include "Parameters.h"
#include "Split.h"
#include "Vocabulary.h"

using namespace std;

static void resetStats(GroupStats& stats) {
    stats.images = 0;
    stats.descriptors = 0;
    stats.minDescriptors = 0;
    stats.maxDescriptors = 0;
}

static void addToStats(GroupStats& stats, int count) {
    if (stats.images == 0 || count < stats.minDescriptors) {
        stats.minDescriptors = count;
    }
    if (stats.images == 0 || count > stats.maxDescriptors) {
        stats.maxDescriptors = count;
    }
    stats.images++;
    stats.descriptors += count;
}

static void printStats(const string& name, const GroupStats& stats) {
    double average = 0;
    if (stats.images > 0) {
        average = (double)stats.descriptors / stats.images;
    }
    printf("  %-22s images %6d  descriptors %10ld  per image: avg %7.1f min %5d max %5d\n",
           name.c_str(), stats.images, stats.descriptors, average,
           stats.minDescriptors, stats.maxDescriptors);
}

static double secondsSince(chrono::steady_clock::time_point start) {
    return chrono::duration<double>(chrono::steady_clock::now() - start).count();
}

// Gets the descriptors of all 6 images of every sequence of one split part
static void loadHpatchesPart(const Parameters& params, const Split& split, const string& part,
                             DescriptorCache& cache) {
    GroupStats stats;
    resetStats(stats);
    DescriptorSet descriptors;

    for (const auto& sequence : split.getSequences(part)) {
        for (int n = 1; n <= 6; n++) {
            string imagePath = params.hpPath + "/" + sequence + "/" + to_string(n) + ".ppm";
            string cacheName = "hpatches/" + sequence + "_" + to_string(n);
            if (!cache.getDescriptors(cacheName, imagePath, descriptors)) {
                fprintf(stderr, "Error: cannot read image %s\n", imagePath.c_str());
                exit(1);
            }
            addToStats(stats, descriptors.getCount());
        }
    }
    printStats("HPatches " + part, stats);
}

// Gets the descriptors of the first D MIRFlickr images that have at least
// one descriptor (im1.jpg, im2.jpg, ...). Returns the chosen file names.
static vector<string> loadMirflickr(const Parameters& params, DescriptorCache& cache) {
    vector<string> chosen;
    vector<string> skipped;
    GroupStats stats;
    resetStats(stats);
    DescriptorSet descriptors;
    chrono::steady_clock::time_point start = chrono::steady_clock::now();

    int number = 1;
    while ((int)chosen.size() < params.numMirflickr) {
        string fileName = "im" + to_string(number) + ".jpg";
        string imagePath = params.mirPath + "/" + fileName;
        string cacheName = "mirflickr/im" + to_string(number);
        if (!cache.getDescriptors(cacheName, imagePath, descriptors)) {
            if (number == 1) {
                fprintf(stderr, "Error: cannot read image %s\n", imagePath.c_str());
                exit(1);
            }
            // No more images: some images had no descriptors, so fewer than
            // D usable images exist (e.g. D = 25000). Use all of them.
            fprintf(stderr, "Warning: only %d usable MIRFlickr images exist, asked for %d\n",
                    (int)chosen.size(), params.numMirflickr);
            break;
        }

        if (descriptors.getCount() == 0) {
            skipped.push_back(fileName);
        } else {
            chosen.push_back(fileName);
            addToStats(stats, descriptors.getCount());
        }

        if (number % 1000 == 0) {
            printf("  ... MIRFlickr image %d, %.0f s\n", number, secondsSince(start));
            fflush(stdout);
        }
        number++;
    }

    printStats("MIRFlickr", stats);
    printf("  MIRFlickr images without descriptors (skipped): %d", (int)skipped.size());
    for (const auto& name : skipped) {
        printf(" %s", name.c_str());
    }
    printf("\n");
    return chosen;
}

// Smallest, largest and average cluster, and how many are empty
static void printClusterSizes(const vector<int>& sizes) {
    int smallest = sizes[0];
    int largest = sizes[0];
    int empty = 0;
    long total = 0;
    for (const auto& size : sizes) {
        if (size < smallest) {
            smallest = size;
        }
        if (size > largest) {
            largest = size;
        }
        if (size == 0) {
            empty++;
        }
        total += size;
    }
    printf("  cluster sizes: min %d, max %d, avg %.1f, empty %d\n",
           smallest, largest, (double)total / sizes.size(), empty);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        Parameters::printUsage(argv[0]);
        return 1;
    }

    Parameters params;
    params.parse(argc, argv);
    params.print();

    Split split(params.splitFile);
    printf("\nSplit: train %d, validation %d, test %d sequences\n",
           (int)split.getSequences("train").size(),
           (int)split.getSequences("validation").size(),
           (int)split.getSequences("test").size());

    // SIFT descriptors of every image this run needs
    printf("\nSIFT descriptors (cache: %s):\n", params.cacheDir.c_str());
    chrono::steady_clock::time_point start = chrono::steady_clock::now();
    DescriptorCache cache(params.cacheDir);

    loadHpatchesPart(params, split, "train", cache);
    loadHpatchesPart(params, split, params.querySet, cache);
    vector<string> mirflickrImages = loadMirflickr(params, cache);

    printf("  images read from cache: %d, extracted now: %d, time %.1f s\n",
           cache.getNumFromCache(), cache.getNumExtracted(), secondsSince(start));

    // Visual vocabulary: K centers from the training descriptors
    printf("\nVocabulary (K = %d, S = %d, seed = %d):\n",
           params.vocabSize, params.maxSiftPerImage, params.seed);
    start = chrono::steady_clock::now();
    Vocabulary vocabulary;
    vocabulary.loadOrBuild(params, split, cache);
    printf("  %d centers, time %.1f s\n", vocabulary.getSize(), secondsSince(start));
    if (!vocabulary.wasLoadedFromCache()) {
        printClusterSizes(vocabulary.getClusterSizes());
    }
    return 0;
}
