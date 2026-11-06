#include "LshTable.h"

using namespace std;

LshTable::LshTable(int k, int dim, double w, int tableSize, mt19937& rng) {
    this->tableSize = tableSize;
    buckets.resize(tableSize);

    // r_i below 2^31, so r_i * (value below M) still fits in a long long
    uniform_int_distribution<long long> randomR(1, 2147483647LL);
    for (int i = 0; i < k; i++) {
        functions.push_back(HashFunction(dim, w, rng));
        r.push_back(randomR(rng));
    }
}

void LshTable::insert(int index, const vector<float>& x) {
    long long id = computeId(x);
    buckets[id % tableSize].push_back(make_pair(id, index));
}

void LshTable::addCandidates(const vector<float>& query, vector<int>& out) const {
    long long id = computeId(query);
    for (const auto& entry : buckets[id % tableSize]) {
        if (entry.first == id) {
            out.push_back(entry.second);
        }
    }
}

int LshTable::countNonEmptyBuckets() const {
    int count = 0;
    for (const auto& bucket : buckets) {
        if (!bucket.empty()) {
            count++;
        }
    }
    return count;
}

// (sum of r_i * h_i) mod M, using (a + b) mod M = ((a mod M) + (b mod M)) mod M
// and keeping every value in [0, M), so nothing overflows a long long:
//  - h_i is an int, so |h_i| < 2^31 < M: "h mod M" is h, or h + M if h < 0
//  - r_i < 2^31 and h < M < 2^32, so r_i * h < 2^63
//  - sum < M and term < M, so sum + term < 2M: one subtraction is enough
long long LshTable::computeId(const vector<float>& x) const {
    long long sum = 0;
    for (int i = 0; i < (int)functions.size(); i++) {
        long long h = functions[i].compute(x);
        if (h < 0) {
            h += M;
        }
        long long term = (r[i] * h) % M;
        sum += term;
        if (sum >= M) {
            sum -= M;
        }
    }
    return sum;
}
