#include "BowEncoder.h"

#include "VectorUtils.h"

using namespace std;

BowEncoder::BowEncoder(const Vocabulary& vocabulary) : vocabulary(vocabulary) {
}

vector<float> BowEncoder::encode(const DescriptorSet& descriptors) const {
    int k = vocabulary.getSize();
    vector<int> counts(k, 0);  // h_1 .. h_K

    vector<float> descriptor;
    float distance;
    for (int i = 0; i < descriptors.getCount(); i++) {
        descriptors.copyDescriptor(i, descriptor);
        int word = nearestCenter(vocabulary.getCenters(), descriptor, distance);
        counts[word]++;
    }

    vector<float> result(k, 0.0f);
    int n = descriptors.getCount();  // n_I
    if (n == 0) {
        return result;
    }
    for (int j = 0; j < k; j++) {
        result[j] = (float)counts[j] / n;
    }
    return result;
}
