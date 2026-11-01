#include "SearchMethod.h"

#include <algorithm>
#include <utility>

#include "VectorUtils.h"

using namespace std;

SearchMethod::SearchMethod() {
    totalCandidates = 0;
    numSearches = 0;
}

SearchMethod::~SearchMethod() {
}

double SearchMethod::getAverageCandidates() const {
    if (numSearches == 0) {
        return 0;
    }
    return (double)totalCandidates / numSearches;
}

vector<int> SearchMethod::rankCandidates(const vector<vector<float> >& vectors,
                                         const vector<float>& query,
                                         const vector<int>& candidates, int topN) {
    totalCandidates += (long)candidates.size();
    numSearches++;

    // (squared distance, index): pairs sort by distance, then by index
    vector<pair<float, int> > scored;
    for (const auto& index : candidates) {
        scored.push_back(make_pair(squaredDistance(vectors[index], query), index));
    }

    int take = min(topN, (int)scored.size());
    partial_sort(scored.begin(), scored.begin() + take, scored.end());

    vector<int> result;
    for (int i = 0; i < take; i++) {
        result.push_back(scored[i].second);
    }
    return result;
}
