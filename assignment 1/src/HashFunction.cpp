#include "HashFunction.h"

#include <climits>
#include <cmath>

using namespace std;

HashFunction::HashFunction(int dim, double w, mt19937& rng) {
    this->w = w;

    normal_distribution<float> gaussian(0.0f, 1.0f);
    v.resize(dim);
    for (int i = 0; i < dim; i++) {
        v[i] = gaussian(rng);
    }

    uniform_real_distribution<double> shift(0.0, w);
    t = shift(rng);
}

int HashFunction::compute(const vector<float>& x) const {
    double dot = 0;
    for (int i = 0; i < (int)v.size(); i++) {
        dot += x[i] * v[i];
    }
    double cell = floor((dot + t) / w);

    // Only a tiny w could give a cell number outside the int range;
    // keep it inside so the conversion to int is always valid.
    if (cell > INT_MAX) {
        cell = INT_MAX;
    }
    if (cell < INT_MIN) {
        cell = INT_MIN;
    }
    return (int)cell;
}
