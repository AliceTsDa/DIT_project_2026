#pragma once

#include <string>
#include <vector>

#include "BowEncoder.h"
#include "DescriptorCache.h"
#include "Parameters.h"

// Gives the vector x_I of an image, computing it only once.
// The vector depends on the vocabulary, so the files are kept per
// (K, S, seed): <cacheDir>/vectors/K<K>_S<S>_seed<seed>/<cacheName>.bin
// (cache names as in DescriptorCache, e.g. "hpatches/v_bird_2").
//
// File format: int K, int n_I (number of SIFT descriptors), then K floats.
// n_I is kept so that images without descriptors can be skipped without
// reading their descriptors again.
class VectorCache {
public:
    VectorCache(const Parameters& params, DescriptorCache& descriptorCache, const BowEncoder& encoder);

    // Fills "result" with x_I and "numDescriptors" with n_I.
    // Returns false if the image is not cached and cannot be read.
    bool getVector(const std::string& cacheName, const std::string& imagePath,
                   std::vector<float>& result, int& numDescriptors);

    int getNumFromCache() const;
    int getNumComputed() const;

private:
    std::string dir;
    int k;
    DescriptorCache& descriptorCache;
    const BowEncoder& encoder;
    int numFromCache;
    int numComputed;

    bool load(const std::string& fileName, std::vector<float>& result, int& numDescriptors) const;
    bool save(const std::string& fileName, const std::vector<float>& values, int numDescriptors) const;
};
