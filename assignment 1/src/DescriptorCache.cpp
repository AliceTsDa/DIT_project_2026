#include "DescriptorCache.h"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>

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

    // Write to a temporary file first and rename it at the end, so that a
    // run stopped in the middle (Ctrl+C) never leaves a half-written file.
    string tempName = fileName + ".tmp";
    if (!result.save(tempName) || rename(tempName.c_str(), fileName.c_str()) != 0) {
        fprintf(stderr, "Error: cannot write cache file %s\n", fileName.c_str());
        exit(1);
    }
    return true;
}

int DescriptorCache::getNumFromCache() const {
    return numFromCache;
}

int DescriptorCache::getNumExtracted() const {
    return numExtracted;
}

void DescriptorCache::makeDirectory(const string& path) {
    // EEXIST: the directory is already there, which is fine
    if (mkdir(path.c_str(), 0755) != 0 && errno != EEXIST) {
        fprintf(stderr, "Error: cannot create directory %s\n", path.c_str());
        exit(1);
    }
}
