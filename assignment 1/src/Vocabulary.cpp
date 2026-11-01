#include "Vocabulary.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <random>

#include "DescriptorSet.h"
#include "FileUtils.h"
#include "KMeans.h"

using namespace std;

Vocabulary::Vocabulary() {
    fromCache = false;
}

void Vocabulary::loadOrBuild(const Parameters& params, const Split& split, DescriptorCache& cache) {
    string dir = params.cacheDir + "/vocab";
    makeDirectory(dir);
    string fileName = dir + "/vocab_K" + to_string(params.vocabSize) +
                      "_S" + to_string(params.maxSiftPerImage) +
                      "_seed" + to_string(params.seed) + ".bin";

    if (load(fileName, params.vocabSize)) {
        fromCache = true;
        printf("  vocabulary loaded from %s\n", fileName.c_str());
        return;
    }

    build(params, split, cache);
    if (!save(tempFileName(fileName))) {
        fprintf(stderr, "Error: cannot write vocabulary file %s\n", fileName.c_str());
        exit(1);
    }
    finishTempFile(fileName);
    printf("  vocabulary saved to %s\n", fileName.c_str());
}

int Vocabulary::getSize() const {
    return (int)centers.size();
}

const vector<vector<float> >& Vocabulary::getCenters() const {
    return centers;
}

const vector<int>& Vocabulary::getClusterSizes() const {
    return clusterSizes;
}

bool Vocabulary::wasLoadedFromCache() const {
    return fromCache;
}

// Takes up to S random descriptors from every training image (all of them
// if the image has fewer) and runs k-means on all of them together.
void Vocabulary::build(const Parameters& params, const Split& split, DescriptorCache& cache) {
    mt19937 rng(params.seed);
    vector<vector<float> > sample;
    DescriptorSet descriptors;

    for (const auto& sequence : split.getSequences("train")) {
        for (int n = 1; n <= 6; n++) {
            string imagePath = params.hpPath + "/" + sequence + "/" + to_string(n) + ".ppm";
            string cacheName = "hpatches/" + sequence + "_" + to_string(n);
            if (!cache.getDescriptors(cacheName, imagePath, descriptors)) {
                fprintf(stderr, "Error: cannot read image %s\n", imagePath.c_str());
                exit(1);
            }

            // shuffle the indices and keep the first S
            vector<int> indices(descriptors.getCount());
            for (int i = 0; i < descriptors.getCount(); i++) {
                indices[i] = i;
            }
            shuffle(indices.begin(), indices.end(), rng);
            int take = min(params.maxSiftPerImage, descriptors.getCount());
            for (int i = 0; i < take; i++) {
                sample.push_back(descriptors.getDescriptor(indices[i]));
            }
        }
    }

    printf("  building vocabulary: K = %d from %d training descriptors (S = %d)\n",
           params.vocabSize, (int)sample.size(), params.maxSiftPerImage);
    KMeans kmeans(params.vocabSize, MAX_ITERATIONS, params.seed, true);
    kmeans.run(sample);
    centers = kmeans.getCenters();
    clusterSizes = kmeans.getClusterSizes();
}

bool Vocabulary::save(const string& fileName) const {
    FILE* file = fopen(fileName.c_str(), "wb");
    if (file == NULL) {
        return false;
    }
    int k = (int)centers.size();
    int dim = DescriptorSet::DIM;
    bool ok = fwrite(&k, sizeof(int), 1, file) == 1 && fwrite(&dim, sizeof(int), 1, file) == 1;
    for (int c = 0; ok && c < k; c++) {
        ok = (int)fwrite(centers[c].data(), sizeof(float), dim, file) == dim;
    }
    fclose(file);
    return ok;
}

bool Vocabulary::load(const string& fileName, int expectedSize) {
    centers.clear();
    FILE* file = fopen(fileName.c_str(), "rb");
    if (file == NULL) {
        return false;
    }
    int k = 0;
    int dim = 0;
    bool ok = fread(&k, sizeof(int), 1, file) == 1 && fread(&dim, sizeof(int), 1, file) == 1 &&
              k == expectedSize && dim == DescriptorSet::DIM;
    for (int c = 0; ok && c < k; c++) {
        vector<float> center(dim);
        ok = (int)fread(center.data(), sizeof(float), dim, file) == dim;
        centers.push_back(center);
    }
    fclose(file);
    if (!ok) {
        centers.clear();
        return false;
    }
    return true;
}
