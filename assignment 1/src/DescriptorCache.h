#pragma once

#include <string>

#include "DescriptorSet.h"
#include "SiftExtractor.h"

// Gives the SIFT descriptors of an image, extracting them only once.
// The first time, the descriptors are extracted and saved to
// <cacheDir>/<cacheName>.bin. Every later time they are read from that file.
// SIFT has no randomness, so the cache does not depend on any parameter.
//
// Cache names look like "hpatches/v_bird_1" or "mirflickr/im1".
class DescriptorCache {
public:
    // Creates <cacheDir>, <cacheDir>/hpatches and <cacheDir>/mirflickr
    DescriptorCache(const std::string& cacheDir);

    // Fills "result" with the descriptors of the image.
    // Returns false if the image is not in the cache and cannot be read.
    bool getDescriptors(const std::string& cacheName, const std::string& imagePath,
                        DescriptorSet& result);

    int getNumFromCache() const;
    int getNumExtracted() const;

private:
    std::string dir;
    SiftExtractor extractor;
    int numFromCache;
    int numExtracted;

    static void makeDirectory(const std::string& path);
};
