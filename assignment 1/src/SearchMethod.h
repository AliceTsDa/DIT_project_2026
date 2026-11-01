#pragma once

#include <string>
#include <vector>

// Base class of all search methods (Exact, LSH, Hypercube, IVFFlat, IVFPQ).
//
// Usage: build() once with the database vectors (index construction, not
// timed), then search() for every query (timed).
class SearchMethod {
public:
    virtual ~SearchMethod();

    // Name written at the top of the output file, e.g. "Exact Search"
    virtual std::string getName() const = 0;

    // Builds the index. The vectors must stay alive while searching.
    virtual void build(const std::vector<std::vector<float> >& vectors) = 0;

    // Indices of (up to) the topN nearest database vectors, nearest first.
    // Approximate methods may return fewer than topN.
    virtual std::vector<int> search(const std::vector<float>& query, int topN) = 0;

protected:
    // Sorts the candidates by their true Euclidean distance to the query and
    // returns the topN nearest. Used by every method for the final ranking.
    static std::vector<int> rankCandidates(const std::vector<std::vector<float> >& vectors,
                                           const std::vector<float>& query,
                                           const std::vector<int>& candidates, int topN);
};
