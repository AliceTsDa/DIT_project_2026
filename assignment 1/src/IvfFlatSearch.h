#pragma once

#include <string>
#include <vector>

#include "SearchMethod.h"

// Inverted File Flat (IVFFlat).
//
// build():  k-means on the database vectors x_I gives kclusters centroids
//           (a different k-means from the SIFT vocabulary). Every database
//           vector goes into the list ("inverted list") of its centroid.
// search(): find the nprobe centroids closest to the query and compare the
//           query only with the vectors in those lists (true distance).
class IvfFlatSearch : public SearchMethod {
public:
    // numClusters = 0 means ceil(sqrt(n)), decided in build()
    IvfFlatSearch(int numClusters, int nprobe, int seed);

    std::string getName() const;
    void build(const std::vector<std::vector<float> >& vectors);
    std::vector<int> search(const std::vector<float>& query, int topN);

    // k-means on the database is not cached, so keep it short
    static const int MAX_ITERATIONS = 25;

protected:
    int numClusters;
    int nprobe;
    int seed;
    const std::vector<std::vector<float> >* data;  // the database vectors
    std::vector<std::vector<float> > centroids;
    std::vector<std::vector<int> > lists;           // lists[c] = vectors of cluster c

    // Builds centroids and lists (kept separate so an IVFPQ could reuse it)
    void buildLists(const std::vector<std::vector<float> >& vectors);

    // The nprobe centroids closest to the query, closest first
    std::vector<int> closestCentroids(const std::vector<float>& query) const;
};
