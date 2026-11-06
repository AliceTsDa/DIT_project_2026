#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "BitFlipper.h"
#include "HashFunction.h"
#include "SearchMethod.h"

// Random projection on the binary hypercube (course slides 27-28).
//
// build():  every point p goes to the hypercube vertex
//             [f_1(h_1(p)), ..., f_d'(h_d'(p))]  in {0,1}^d'
//           stored as an int (bit i = f_i(h_i(p))). The vertices are the
//           buckets; only non-empty vertices are kept.
// search(): check the points of the query's vertex, then of the vertices at
//           Hamming distance 1, 2, ... from it, until maxCandidates points
//           (-M) or maxProbes vertices (-probes) have been checked.
//           The candidates are ranked by true distance.
class HypercubeSearch : public SearchMethod {
public:
    HypercubeSearch(int dimension, double w, int maxCandidates, int maxProbes, int seed);

    std::string getName() const;
    void build(const std::vector<std::vector<float> >& vectors);
    std::vector<int> search(const std::vector<float>& query, int topN);

private:
    int dimension;  // d' (-kproj)
    double w;
    int maxCandidates;
    int maxProbes;
    int seed;
    const std::vector<std::vector<float> >* data;  // the database vectors

    std::vector<HashFunction> functions;  // h_1 .. h_d'
    std::vector<BitFlipper> flippers;     // f_1 .. f_d'
    std::unordered_map<int, std::vector<int> > vertices;  // vertex -> points

    // probeOrder[j] = bits to flip for the j-th probe: 0 (the query's own
    // vertex), then all masks with 1 bit, then 2 bits, ... (maxProbes of them)
    std::vector<int> probeOrder;

    int computeVertex(const std::vector<float>& x);
    void buildProbeOrder();
    void addMasks(int start, int bitsLeft, int mask);
};
