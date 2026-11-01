#include "ImageDatabase.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>

using namespace std;

ImageDatabase::ImageDatabase() {
    numHpatches = 0;
}

void ImageDatabase::build(const Parameters& params, const Split& split, VectorCache& cache) {
    addHpatches(params, split, cache);
    addMirflickr(params, cache);
}

int ImageDatabase::getSize() const {
    return (int)vectors.size();
}

int ImageDatabase::getNumHpatches() const {
    return numHpatches;
}

int ImageDatabase::getNumMirflickr() const {
    return (int)vectors.size() - numHpatches;
}

const vector<vector<float> >& ImageDatabase::getVectors() const {
    return vectors;
}

const vector<float>& ImageDatabase::getVector(int i) const {
    return vectors[i];
}

const string& ImageDatabase::getImageId(int i) const {
    return imageIds[i];
}

const string& ImageDatabase::getImageSequence(int i) const {
    return imageSequences[i];
}

int ImageDatabase::getNumQueries() const {
    return (int)queryVectors.size();
}

const vector<float>& ImageDatabase::getQueryVector(int q) const {
    return queryVectors[q];
}

const string& ImageDatabase::getQuerySequence(int q) const {
    return querySequences[q];
}

bool ImageDatabase::isRelevant(int q, int i) const {
    return imageSequences[i] == querySequences[q];
}

const vector<string>& ImageDatabase::getSkippedMirflickr() const {
    return skippedMirflickr;
}

// Image 1 of every sequence of the query set is a query, images 2..6 go to
// the database.
void ImageDatabase::addHpatches(const Parameters& params, const Split& split, VectorCache& cache) {
    vector<float> x;
    int numDescriptors;

    for (const auto& sequence : split.getSequences(params.querySet)) {
        for (int n = 1; n <= 6; n++) {
            string imagePath = params.hpPath + "/" + sequence + "/" + to_string(n) + ".ppm";
            string cacheName = "hpatches/" + sequence + "_" + to_string(n);
            if (!cache.getVector(cacheName, imagePath, x, numDescriptors)) {
                fprintf(stderr, "Error: cannot read image %s\n", imagePath.c_str());
                exit(1);
            }

            if (n == 1) {
                queryVectors.push_back(x);
                querySequences.push_back(sequence);
            } else {
                vectors.push_back(x);
                imageIds.push_back(sequence + "/" + to_string(n));
                imageSequences.push_back(sequence);
            }
        }
    }
    numHpatches = (int)vectors.size();
}

// The first D MIRFlickr images (im1.jpg, im2.jpg, ...) that have at least
// one descriptor. Images without descriptors are skipped and replaced by
// the next one.
void ImageDatabase::addMirflickr(const Parameters& params, VectorCache& cache) {
    vector<float> x;
    int numDescriptors;
    int added = 0;
    chrono::steady_clock::time_point start = chrono::steady_clock::now();

    int number = 1;
    while (added < params.numMirflickr) {
        string fileName = "im" + to_string(number) + ".jpg";
        string imagePath = params.mirPath + "/" + fileName;
        string cacheName = "mirflickr/im" + to_string(number);
        if (!cache.getVector(cacheName, imagePath, x, numDescriptors)) {
            if (number == 1) {
                fprintf(stderr, "Error: cannot read image %s\n", imagePath.c_str());
                exit(1);
            }
            // No more images: some images had no descriptors, so fewer than
            // D usable images exist (e.g. D = 25000). Use all of them.
            fprintf(stderr, "Warning: only %d usable MIRFlickr images exist, asked for %d\n",
                    added, params.numMirflickr);
            break;
        }

        if (numDescriptors == 0) {
            skippedMirflickr.push_back(fileName);
        } else {
            vectors.push_back(x);
            imageIds.push_back(fileName);
            imageSequences.push_back("");
            added++;
        }

        if (number % 1000 == 0) {
            double seconds = chrono::duration<double>(chrono::steady_clock::now() - start).count();
            printf("  ... MIRFlickr image %d, %.0f s\n", number, seconds);
            fflush(stdout);
        }
        number++;
    }
}
