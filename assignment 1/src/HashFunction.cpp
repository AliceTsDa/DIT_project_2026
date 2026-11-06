#include "HashFunction.h"

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
    return (int)floor((dot + t) / w);
}
