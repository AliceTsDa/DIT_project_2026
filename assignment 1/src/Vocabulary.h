#pragma once

#include <string>
#include <vector>

#include "DescriptorCache.h"
#include "Parameters.h"
#include "Split.h"

// The visual vocabulary: the K k-means centers c1..cK of SIFT descriptors
// (each a "visual word", 128 values).
//
// Built from up to S random descriptors of every training image, then
// saved to <cacheDir>/vocab/vocab_K<K>_S<S>_seed<seed>.bin, so k-means runs
// only once for every (K, S, seed).
class Vocabulary {
public:
    Vocabulary();

    // Loads the vocabulary from the cache, or builds and saves it
    void loadOrBuild(const Parameters& params, const Split& split, DescriptorCache& cache);

    int getSize() const;
    const std::vector<std::vector<float> >& getCenters() const;

    // Cluster sizes from k-means (only known if built in this run)
    const std::vector<int>& getClusterSizes() const;

    bool wasLoadedFromCache() const;

private:
    std::vector<std::vector<float> > centers;
    std::vector<int> clusterSizes;
    bool fromCache;

    static const int MAX_ITERATIONS = 100;

    void build(const Parameters& params, const Split& split, DescriptorCache& cache);

    // Binary file: int K, int dim, then K * dim floats
    bool save(const std::string& fileName) const;
    bool load(const std::string& fileName, int expectedSize);
};
