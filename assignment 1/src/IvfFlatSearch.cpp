#include "IvfFlatSearch.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <utility>

#include "KMeans.h"
#include "VectorUtils.h"

using namespace std;

IvfFlatSearch::IvfFlatSearch(int numClusters, int nprobe, int seed) {
    this->numClusters = numClusters;
    this->nprobe = nprobe;
    this->seed = seed;
    data = NULL;
}

string IvfFlatSearch::getName() const {
    return "IVFFlat";
}

void IvfFlatSearch::build(const vector<vector<float> >& vectors) {
    buildLists(vectors);
}

vector<int> IvfFlatSearch::search(const vector<float>& query, int topN) {
    vector<int> candidates;
    for (const auto& c : closestCentroids(query)) {
        for (const auto& index : lists[c]) {
            candidates.push_back(index);
        }
    }
    return rankCandidates(*data, query, candidates, topN);
}

void IvfFlatSearch::buildLists(const vector<vector<float> >& vectors) {
    data = &vectors;
    int n = (int)vectors.size();
    if (numClusters == 0) {
        numClusters = (int)ceil(sqrt((double)n));
    }
    if (numClusters > n) {
        printf("  -kclusters %d is more than the %d vectors, using %d\n", numClusters, n, n);
        numClusters = n;
    }
    if (nprobe > numClusters) {
        printf("  -nprobe %d is more than the %d clusters, using %d\n", nprobe, numClusters, numClusters);
        nprobe = numClusters;
    }

    KMeans kmeans(numClusters, MAX_ITERATIONS, seed, false);
    kmeans.run(vectors);
    centroids = kmeans.getCenters();

    lists.assign(numClusters, vector<int>());
    const vector<int>& assignments = kmeans.getAssignments();
    for (int i = 0; i < n; i++) {
        lists[assignments[i]].push_back(i);
    }

    // list sizes, to see how even the clusters are
    int smallest = n;
    int largest = 0;
    int empty = 0;
    for (const auto& list : lists) {
        int size = (int)list.size();
        smallest = min(smallest, size);
        largest = max(largest, size);
        if (size == 0) {
            empty++;
        }
    }
    printf("  kclusters %d, nprobe %d: list sizes min %d, max %d, avg %.1f, empty %d\n",
           numClusters, nprobe, smallest, largest, (double)n / numClusters, empty);
}

vector<int> IvfFlatSearch::closestCentroids(const vector<float>& query) const {
    vector<pair<float, int> > scored;
    for (int c = 0; c < (int)centroids.size(); c++) {
        scored.push_back(make_pair(squaredDistance(centroids[c], query), c));
    }
    partial_sort(scored.begin(), scored.begin() + nprobe, scored.end());

    vector<int> result;
    for (int i = 0; i < nprobe; i++) {
        result.push_back(scored[i].second);
    }
    return result;
}
