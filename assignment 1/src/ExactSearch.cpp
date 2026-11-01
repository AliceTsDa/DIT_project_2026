#include "ExactSearch.h"

#include <cstddef>

using namespace std;

ExactSearch::ExactSearch() {
    data = NULL;
}

string ExactSearch::getName() const {
    return "Exact Search";
}

void ExactSearch::build(const vector<vector<float> >& vectors) {
    data = &vectors;
    allIndices.clear();
    for (int i = 0; i < (int)vectors.size(); i++) {
        allIndices.push_back(i);
    }
}

vector<int> ExactSearch::search(const vector<float>& query, int topN) {
    // every database vector is a candidate
    return rankCandidates(*data, query, allIndices, topN);
}
