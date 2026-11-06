#pragma once

#include <random>
#include <vector>

// One hash function of the Euclidean LSH family:
//
//   h(x) = floor((x . v + t) / w)
//
// v: random direction, every value from N(0, 1)
// t: random shift, uniform in [0, w)
// w: cell width on the projection line
//
// Close vectors project to close points on the line along v, so they
// usually fall in the same cell and get the same integer.
// Used by LSH and by the Hypercube.
class HashFunction {
public:
    HashFunction(int dim, double w, std::mt19937& rng);

    int compute(const std::vector<float>& x) const;

private:
    std::vector<float> v;
    double t;
    double w;
};
