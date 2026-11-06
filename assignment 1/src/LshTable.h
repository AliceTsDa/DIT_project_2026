#pragma once

#include <random>
#include <utility>
#include <vector>

#include "HashFunction.h"

// One LSH hash table with its function g (the Euclidean LSH of the course):
//
//   ID(x) = (r_1 h_1(x) + ... + r_k h_k(x)) mod M,   M = 2^32 - 5
//   g(x)  = ID(x) mod T                             (bucket number)
//
// h_i: k random HashFunctions, r_i: k random integers, T: number of buckets.
// Every bucket keeps (ID, index) pairs. A query only takes the vectors of
// its bucket that have the same ID ("compare x, q iff g(x) = g(q)"), which
// skips vectors that share the bucket only because of "mod T".
class LshTable {
public:
    static const long long M = 4294967291LL;  // 2^32 - 5, a prime

    LshTable(int k, int dim, double w, int tableSize, std::mt19937& rng);

    // Puts database vector "index" into its bucket
    void insert(int index, const std::vector<float>& x);

    // Appends to "out" the indices in the query's bucket with the same ID
    void addCandidates(const std::vector<float>& query, std::vector<int>& out) const;

    // Number of buckets that are not empty (for statistics)
    int countNonEmptyBuckets() const;

private:
    std::vector<HashFunction> functions;
    std::vector<long long> r;
    int tableSize;
    std::vector<std::vector<std::pair<long long, int> > > buckets;

    long long computeId(const std::vector<float>& x) const;
};
