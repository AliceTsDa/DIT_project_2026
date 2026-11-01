#include "VectorCache.h"

#include <cstdio>
#include <cstdlib>

#include "DescriptorSet.h"
#include "FileUtils.h"

using namespace std;

VectorCache::VectorCache(const Parameters& params, DescriptorCache& descriptorCache,
                         const BowEncoder& encoder)
    : descriptorCache(descriptorCache), encoder(encoder) {
    k = params.vocabSize;
    numFromCache = 0;
    numComputed = 0;

    makeDirectory(params.cacheDir + "/vectors");
    dir = params.cacheDir + "/vectors/K" + to_string(params.vocabSize) +
          "_S" + to_string(params.maxSiftPerImage) + "_seed" + to_string(params.seed);
    makeDirectory(dir);
    makeDirectory(dir + "/hpatches");
    makeDirectory(dir + "/mirflickr");
}

bool VectorCache::getVector(const string& cacheName, const string& imagePath,
                            vector<float>& result, int& numDescriptors) {
    string fileName = dir + "/" + cacheName + ".bin";
    if (load(fileName, result, numDescriptors)) {
        numFromCache++;
        return true;
    }

    DescriptorSet descriptors;
    if (!descriptorCache.getDescriptors(cacheName, imagePath, descriptors)) {
        return false;
    }
    result = encoder.encode(descriptors);
    numDescriptors = descriptors.getCount();
    numComputed++;

    // write to a temporary file first, then rename (see FileUtils.h)
    if (!save(tempFileName(fileName), result, numDescriptors)) {
        fprintf(stderr, "Error: cannot write vector file %s\n", fileName.c_str());
        exit(1);
    }
    finishTempFile(fileName);
    return true;
}

int VectorCache::getNumFromCache() const {
    return numFromCache;
}

int VectorCache::getNumComputed() const {
    return numComputed;
}

bool VectorCache::load(const string& fileName, vector<float>& result, int& numDescriptors) const {
    FILE* file = fopen(fileName.c_str(), "rb");
    if (file == NULL) {
        return false;
    }
    int fileK = 0;
    int fileN = 0;
    bool ok = fread(&fileK, sizeof(int), 1, file) == 1 && fread(&fileN, sizeof(int), 1, file) == 1 &&
              fileK == k && fileN >= 0;
    if (ok) {
        result.resize(k);
        ok = (int)fread(result.data(), sizeof(float), k, file) == k;
    }
    fclose(file);
    if (!ok) {
        return false;
    }
    numDescriptors = fileN;
    return true;
}

bool VectorCache::save(const string& fileName, const vector<float>& values, int numDescriptors) const {
    FILE* file = fopen(fileName.c_str(), "wb");
    if (file == NULL) {
        return false;
    }
    bool ok = fwrite(&k, sizeof(int), 1, file) == 1 &&
              fwrite(&numDescriptors, sizeof(int), 1, file) == 1 &&
              (int)fwrite(values.data(), sizeof(float), k, file) == k;
    fclose(file);
    return ok;
}
