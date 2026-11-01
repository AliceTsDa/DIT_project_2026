#pragma once

#include <string>
#include <vector>

// The result of one query: its ranking and its metrics
struct QueryResult {
    std::string sequence;      // query sequence, e.g. "v_bird"
    std::vector<int> ranking;  // database indices, nearest first (up to 10)
    double recall5;
    double recall10;
    double ap10;
    double timeMs;             // search time in milliseconds
};
