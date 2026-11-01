#pragma once

#include <vector>

#include "DescriptorSet.h"
#include "Vocabulary.h"

// Turns the SIFT descriptors of an image into its "bag of visual words"
// vector x_I = (h_1 / n_I, ..., h_K / n_I):
// every descriptor is assigned to its nearest vocabulary center, h_j counts
// the descriptors of center j and n_I is the number of descriptors.
// The values are >= 0 and sum to 1.
class BowEncoder {
public:
    BowEncoder(const Vocabulary& vocabulary);

    // An image without descriptors gives the all-zero vector
    std::vector<float> encode(const DescriptorSet& descriptors) const;

private:
    const Vocabulary& vocabulary;
};
