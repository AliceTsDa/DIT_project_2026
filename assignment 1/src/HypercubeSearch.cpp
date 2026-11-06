#include "HypercubeSearch.h"

#include <cstddef>
#include <cstdio>
#include <random>

using namespace std;

HypercubeSearch::HypercubeSearch(int dimension, double w, int maxCandidates, int maxProbes, int seed) {
    this->dimension = dimension;
    this->w = w;
    this->maxCandidates = maxCandidates;
    this->maxProbes = maxProbes;
    this->seed = seed;
    data = NULL;
}

string HypercubeSearch::getName() const {
    return "Hypercube";
}

void HypercubeSearch::build(const vector<vector<float> >& vectors) {
    data = &vectors;
    int n = (int)vectors.size();
    int dim = (int)vectors[0].size();

    mt19937 rng(seed);
    functions.clear();
    flippers.clear();
    for (int i = 0; i < dimension; i++) {
        functions.push_back(HashFunction(dim, w, rng));
        flippers.push_back(BitFlipper(rng()));  // every f_i its own random bits
    }

    vertices.clear();
    for (int i = 0; i < n; i++) {
        vertices[computeVertex(vectors[i])].push_back(i);
    }

    long totalVertices = 1L << dimension;
    if (maxProbes > totalVertices) {
        printf("  -probes %d is more than the %ld vertices, using %ld\n",
               maxProbes, totalVertices, totalVertices);
        maxProbes = (int)totalVertices;
    }
    buildProbeOrder();

    printf("  kproj %d, w %g, M %d, probes %d: %ld vertices, non-empty %d, avg points per non-empty %.1f\n",
           dimension, w, maxCandidates, maxProbes, totalVertices, (int)vertices.size(),
           (double)n / vertices.size());
}

vector<int> HypercubeSearch::search(const vector<float>& query, int topN) {
    int queryVertex = computeVertex(query);

    // Every point is in exactly one vertex, so no duplicates are possible
    vector<int> candidates;
    for (const auto& mask : probeOrder) {
        int vertex = queryVertex ^ mask;  // flip the bits of the mask
        if (vertices.count(vertex) == 0) {
            continue;  // empty vertex (still counts as a probe)
        }
        for (const auto& index : vertices[vertex]) {
            candidates.push_back(index);
            if ((int)candidates.size() >= maxCandidates) {
                return rankCandidates(*data, query, candidates, topN);
            }
        }
    }
    return rankCandidates(*data, query, candidates, topN);
}

// bit i of the vertex = f_i(h_i(x))
int HypercubeSearch::computeVertex(const vector<float>& x) {
    int vertex = 0;
    for (int i = 0; i < dimension; i++) {
        int bit = flippers[i].getBit(functions[i].compute(x));
        vertex = vertex | (bit << i);
    }
    return vertex;
}

// Masks in increasing Hamming distance: distance 0, then 1, then 2, ...
// until there are maxProbes of them.
void HypercubeSearch::buildProbeOrder() {
    probeOrder.clear();
    for (int distance = 0; distance <= dimension; distance++) {
        addMasks(0, distance, 0);
        if ((int)probeOrder.size() >= maxProbes) {
            break;
        }
    }
}

// Adds all masks that have "bitsLeft" more 1-bits at positions >= start.
// Stops as soon as there are maxProbes masks.
void HypercubeSearch::addMasks(int start, int bitsLeft, int mask) {
    if ((int)probeOrder.size() >= maxProbes) {
        return;
    }
    if (bitsLeft == 0) {
        probeOrder.push_back(mask);
        return;
    }
    for (int bit = start; bit < dimension; bit++) {
        addMasks(bit + 1, bitsLeft - 1, mask | (1 << bit));
    }
}
