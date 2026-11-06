#pragma once

#include <random>
#include <vector>

// K-means clustering (Lloyd's algorithm) with k-means++ initialisation.
// Used for the SIFT vocabulary and for the IVFFlat index.
//
// Stops after maxIterations, or earlier when fewer than 0.1% of the points
// change cluster in an iteration.
class KMeans {
public:
    KMeans(int k, int maxIterations, int seed, bool verbose);

    // Clusters the points. All points must have the same dimension.
    void run(const std::vector<std::vector<float> >& points);

    const std::vector<std::vector<float> >& getCenters() const;

    // Cluster of every point after run()
    const std::vector<int>& getAssignments() const;

    // Number of points in every cluster after run()
    std::vector<int> getClusterSizes() const;

    // Mean squared distance of the points to their center after run()
    double getError() const;

private:
    int k;
    int maxIterations;
    bool verbose;
    std::mt19937 rng;

    std::vector<std::vector<float> > centers;
    std::vector<int> assignments;
    double error;

    void initPlusPlus(const std::vector<std::vector<float> >& points);
    int assignPoints(const std::vector<std::vector<float> >& points);
    void updateCenters(const std::vector<std::vector<float> >& points);
};
