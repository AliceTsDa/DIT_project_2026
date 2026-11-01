#include "DescriptorCache.h"

#include <cstdio>
#include <cstdlib>

#include "FileUtils.h"

using namespace std;

DescriptorCache::DescriptorCache(const string& cacheDir) {
    dir = cacheDir;
    numFromCache = 0;
    numExtracted = 0;
    makeDirectory(dir);
    makeDirectory(dir + "/hpatches");
    makeDirectory(dir + "/mirflickr");
}

bool DescriptorCache::getDescriptors(const string& cacheName, const string& imagePath,
                                     DescriptorSet& result) {
    string fileName = dir + "/" + cacheName + ".bin";
    if (result.load(fileName)) {
        numFromCache++;
        return true;
    }

    if (!extractor.extract(imagePath, result)) {
        return false;
    }
    numExtracted++;

    // write to a temporary file first, then rename (see FileUtils.h)
    if (!result.save(tempFileName(fileName))) {
        fprintf(stderr, "Error: cannot write cache file %s\n", fileName.c_str());
        exit(1);
    }
    finishTempFile(fileName);
    return true;
}

int DescriptorCache::getNumFromCache() const {
    return numFromCache;
}

int DescriptorCache::getNumExtracted() const {
    return numExtracted;
}
