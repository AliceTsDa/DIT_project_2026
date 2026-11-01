#pragma once

#include <string>
#include <vector>

#include "ImageDatabase.h"
#include "MetricsSummary.h"
#include "QueryResult.h"

// Computes the metrics of every query and their means.
//
// R(Q) = the 5 relevant images of query Q, Top_n(Q) = first n results.
//   Recall@n(Q) = |Top_n(Q) ∩ R(Q)| / 5
//   AP@10(Q)    = (1/5) * sum over the relevant results in the top 10,
//                 at positions r_1 < ... < r_m, of j / r_j
// Missing positions (fewer than 10 results) count as not relevant.
class Evaluator {
public:
    static const int NUM_RELEVANT = 5;

    Evaluator(const ImageDatabase& database);

    // Computes and stores the metrics of query q
    void addQuery(int q, const std::vector<int>& ranking, double timeMs);

    const std::vector<QueryResult>& getResults() const;

    // group: "all", "illumination" (i_ sequences) or "viewpoint" (v_)
    MetricsSummary getSummary(const std::string& group) const;

private:
    const ImageDatabase& database;
    std::vector<QueryResult> results;

    double recallAt(int q, const std::vector<int>& ranking, int n) const;
    double averagePrecisionAt10(int q, const std::vector<int>& ranking) const;
};
