#pragma once

#include <string>
#include <vector>

#include "LshTable.h"
#include "SearchMethod.h"

// Locality Sensitive Hashing with L hash tables (see LshTable).
//
// build():  every database vector goes into one bucket of each table.
// search(): the candidates are the vectors in the query's bucket (same ID)
//           in any of the L tables; they are ranked by true distance.
//           With few candidates a query may get fewer than 10 results.
class LshSearch : public SearchMethod {
public:
    // Number of buckets of every table: n / TABLE_SIZE_DIVISOR
    // (the course suggests n/2 ... n/8)
    static const int TABLE_SIZE_DIVISOR = 8;

    LshSearch(int k, int L, double w, int seed);

    std::string getName() const;
    void build(const std::vector<std::vector<float> >& vectors);
    std::vector<int> search(const std::vector<float>& query, int topN);

private:
    int k;
    int numTables;
    double w;
    int seed;
    const std::vector<std::vector<float> >* data;  // the database vectors
    std::vector<LshTable> tables;

    // lastSeen[i] = number of the last search that took vector i as a
    // candidate; removes duplicates (a vector can be found in several tables)
    std::vector<int> lastSeen;
    int searchNumber;
};
