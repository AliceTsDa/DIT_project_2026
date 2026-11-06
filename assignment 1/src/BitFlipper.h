#pragma once

#include <random>
#include <unordered_map>

// One function f_i of the Hypercube: maps every value of a hash function h_i
// to a random bit 0 or 1 (uniformly). The bit of a value is chosen the first
// time the value is seen and remembered, so the same h_i value always gives
// the same bit (for the database points and for the queries).
class BitFlipper {
public:
    BitFlipper(unsigned int seed);

    // Not const: an unseen value gets a new random bit
    int getBit(int hashValue);

private:
    std::mt19937 rng;
    std::unordered_map<int, int> bits;
};
