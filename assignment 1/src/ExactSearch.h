#pragma once

#include <string>
#include <vector>

#include "SearchMethod.h"

// Exhaustive search: compares the query with every database vector.
// Always finds the true nearest neighbours; the baseline for the others.
class ExactSearch : public SearchMethod {
public:
    ExactSearch();

    std::string getName() const;
    void build(const std::vector<std::vector<float> >& vectors);
    std::vector<int> search(const std::vector<float>& query, int topN);

private:
    const std::vector<std::vector<float> >* data;  // the database vectors
    std::vector<int> allIndices;                   // 0, 1, ..., n-1
};
