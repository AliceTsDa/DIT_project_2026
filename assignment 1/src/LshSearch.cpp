#include "LshSearch.h"

#include <cstddef>
#include <cstdio>
#include <random>

using namespace std;

LshSearch::LshSearch(int k, int L, double w, int maxCandidates, int seed) {
    this->k = k;
    this->numTables = L;
    this->w = w;
    this->maxCandidates = maxCandidates;
    this->seed = seed;
    data = NULL;
    searchNumber = 0;
}

string LshSearch::getName() const {
    return "LSH";
}

void LshSearch::build(const vector<vector<float> >& vectors) {
    data = &vectors;
    int n = (int)vectors.size();
    int dim = (int)vectors[0].size();
    int tableSize = n / TABLE_SIZE_DIVISOR;
    if (tableSize < 1) {
        tableSize = 1;
    }

    mt19937 rng(seed);
    tables.clear();
    for (int l = 0; l < numTables; l++) {
        tables.push_back(LshTable(k, dim, w, tableSize, rng));
    }
    for (int i = 0; i < n; i++) {
        for (int l = 0; l < numTables; l++) {
            tables[l].insert(i, vectors[i]);
        }
    }

    lastSeen.assign(n, -1);
    searchNumber = 0;

    // how spread out the vectors are: average over the tables
    double nonEmpty = 0;
    for (const auto& table : tables) {
        nonEmpty += table.countNonEmptyBuckets();
    }
    nonEmpty /= numTables;
    printf("  k %d, L %d, w %g, buckets per table %d: non-empty %.1f, avg vectors per non-empty bucket %.1f\n",
           k, numTables, w, tableSize, nonEmpty, n / nonEmpty);
}

vector<int> LshSearch::search(const vector<float>& query, int topN) {
    searchNumber++;

    vector<int> candidates;
    vector<int> found;
    for (const auto& table : tables) {
        found.clear();
        table.addCandidates(query, found);

        // keep every vector once; stop early if there is a limit
        for (const auto& index : found) {
            if (lastSeen[index] != searchNumber) {
                lastSeen[index] = searchNumber;
                candidates.push_back(index);
                if (maxCandidates > 0 && (int)candidates.size() >= maxCandidates) {
                    return rankCandidates(*data, query, candidates, topN);
                }
            }
        }
    }
    return rankCandidates(*data, query, candidates, topN);
}
