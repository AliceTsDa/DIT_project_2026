// Image search with vector representations (assignment 1).
// Step 4: read the parameters and the split, build (or load) the
// vocabulary, and build the queries and the database of image vectors.
// SIFT descriptors, vocabulary and vectors are all cached.

#include <chrono>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

#include "BowEncoder.h"
#include "DescriptorCache.h"
#include "ImageDatabase.h"
#include "Parameters.h"
#include "Split.h"
#include "VectorCache.h"
#include "Vocabulary.h"

using namespace std;

static double secondsSince(chrono::steady_clock::time_point start) {
    return chrono::duration<double>(chrono::steady_clock::now() - start).count();
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

// Checks that every vector has values >= 0 that sum to 1, and prints how
// many of its K values are non-zero (how many different visual words).
static void checkVectors(const string& name, const vector<vector<float> >& vectors) {
    double worstSumError = 0;
    int negative = 0;
    long nonZero = 0;
    int minNonZero = 0;
    int maxNonZero = 0;

    for (int i = 0; i < (int)vectors.size(); i++) {
        double sum = 0;
        int count = 0;
        for (const auto& value : vectors[i]) {
            sum += value;
            if (value < 0) {
                negative++;
            }
            if (value > 0) {
                count++;
            }
        }
        if (fabs(sum - 1.0) > worstSumError) {
            worstSumError = fabs(sum - 1.0);
        }
        if (i == 0 || count < minNonZero) {
            minNonZero = count;
        }
        if (i == 0 || count > maxNonZero) {
            maxNonZero = count;
        }
        nonZero += count;
    }

    printf("  %-9s %6d vectors: |sum - 1| <= %.1e, negative values %d, non-zero words avg %.1f (min %d, max %d)\n",
           name.c_str(), (int)vectors.size(), worstSumError, negative,
           (double)nonZero / vectors.size(), minNonZero, maxNonZero);
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
    DescriptorCache descriptorCache(params.cacheDir);

    // Visual vocabulary: K centers from the training descriptors
    printf("\nVocabulary (K = %d, S = %d, seed = %d):\n",
           params.vocabSize, params.maxSiftPerImage, params.seed);
    chrono::steady_clock::time_point start = chrono::steady_clock::now();
    Vocabulary vocabulary;
    vocabulary.loadOrBuild(params, split, descriptorCache);
    printf("  %d centers, time %.1f s\n", vocabulary.getSize(), secondsSince(start));
    if (!vocabulary.wasLoadedFromCache()) {
        printClusterSizes(vocabulary.getClusterSizes());
    }

    // Queries and database of image vectors
    printf("\nImage vectors (set %s, D = %d):\n", params.querySet.c_str(), params.numMirflickr);
    start = chrono::steady_clock::now();
    BowEncoder encoder(vocabulary);
    VectorCache vectorCache(params, descriptorCache, encoder);
    ImageDatabase database;
    database.build(params, split, vectorCache);

    printf("  queries:  %d\n", database.getNumQueries());
    printf("  database: %d images (%d HPatches + %d MIRFlickr)\n",
           database.getSize(), database.getNumHpatches(), database.getNumMirflickr());
    printf("  MIRFlickr images skipped (no descriptors): %d\n",
           (int)database.getSkippedMirflickr().size());
    printf("  vectors read from cache: %d, computed now: %d, time %.1f s\n",
           vectorCache.getNumFromCache(), vectorCache.getNumComputed(), secondsSince(start));

    vector<vector<float> > queryVectors;
    for (int q = 0; q < database.getNumQueries(); q++) {
        queryVectors.push_back(database.getQueryVector(q));
    }
    checkVectors("queries", queryVectors);
    checkVectors("database", database.getVectors());
    return 0;
}
