#pragma once

#include <string>
#include <vector>

// Base class of the search methods (Exact, LSH, Hypercube, IVFFlat).
//
// Usage: build() once with the database vectors (index construction, not
// timed), then search() for every query (timed).
class SearchMethod {
public:
    SearchMethod();
    virtual ~SearchMethod();

    // Name written at the top of the output file, e.g. "Exact Search"
    virtual std::string getName() const = 0;

    // Builds the index. The vectors must stay alive while searching.
    virtual void build(const std::vector<std::vector<float> >& vectors) = 0;

    // Indices of (up to) the topN nearest database vectors, nearest first.
    // Approximate methods may return fewer than topN.
    virtual std::vector<int> search(const std::vector<float>& query, int topN) = 0;

    // How many database vectors a search compared on average (the work
    // done; Exact Search compares all of them)
    double getAverageCandidates() const;

protected:
    // Sorts the candidates by their true Euclidean distance to the query and
    // returns the topN nearest. Used by every method for the final ranking.
    // Also counts the candidates for getAverageCandidates().
    std::vector<int> rankCandidates(const std::vector<std::vector<float> >& vectors,
                                    const std::vector<float>& query,
                                    const std::vector<int>& candidates, int topN);

private:
    long totalCandidates;
    int numSearches;
};
