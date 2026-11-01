#pragma once

#include <string>
#include <vector>

#include "Parameters.h"
#include "Split.h"
#include "VectorCache.h"

// The queries and the search database of one run.
//
// Queries:  image I1 of every sequence of the query set (-set).
// Database: images I2..I6 of the same sequences, then the first D MIRFlickr
//           images that have at least one SIFT descriptor (distractors).
//
// A database image is relevant to a query when it comes from the same
// HPatches sequence. MIRFlickr images are never relevant.
class ImageDatabase {
public:
    ImageDatabase();

    void build(const Parameters& params, const Split& split, VectorCache& cache);

    // Database
    int getSize() const;
    int getNumHpatches() const;
    int getNumMirflickr() const;
    const std::vector<std::vector<float> >& getVectors() const;
    const std::vector<float>& getVector(int i) const;
    const std::string& getImageId(int i) const;        // "v_bird/2" or "im123.jpg"
    const std::string& getImageSequence(int i) const;  // "v_bird", or "" for MIRFlickr

    // Queries
    int getNumQueries() const;
    const std::vector<float>& getQueryVector(int q) const;
    const std::string& getQuerySequence(int q) const;  // e.g. "v_bird"

    bool isRelevant(int q, int i) const;

    // MIRFlickr images skipped because they have no descriptors
    const std::vector<std::string>& getSkippedMirflickr() const;

private:
    std::vector<std::vector<float> > vectors;
    std::vector<std::string> imageIds;
    std::vector<std::string> imageSequences;
    int numHpatches;

    std::vector<std::vector<float> > queryVectors;
    std::vector<std::string> querySequences;

    std::vector<std::string> skippedMirflickr;

    void addHpatches(const Parameters& params, const Split& split, VectorCache& cache);
    void addMirflickr(const Parameters& params, VectorCache& cache);
};
