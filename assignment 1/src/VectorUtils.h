#pragma once

#include <vector>

// Free helper functions for vectors of floats.

// Squared Euclidean distance (no square root: faster, same order)
float squaredDistance(const std::vector<float>& a, const std::vector<float>& b);

// Euclidean distance
double euclideanDistance(const std::vector<float>& a, const std::vector<float>& b);

// Index of the center closest to "point". Also gives its squared distance.
int nearestCenter(const std::vector<std::vector<float> >& centers, const std::vector<float>& point,
                  float& bestDistance);
