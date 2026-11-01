#include "KMeans.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>

#include "VectorUtils.h"

using namespace std;

KMeans::KMeans(int k, int maxIterations, int seed, bool verbose) : rng(seed) {
    this->k = k;
    this->maxIterations = maxIterations;
    this->verbose = verbose;
    error = 0;
}

void KMeans::run(const vector<vector<float> >& points) {
    int n = (int)points.size();
    if (n < k) {
        fprintf(stderr, "Error: k-means with %d clusters needs at least %d points (got %d)\n", k, k, n);
        exit(1);
    }

    chrono::steady_clock::time_point start = chrono::steady_clock::now();
    initPlusPlus(points);
    assignments.assign(n, -1);
    if (verbose) {
        double seconds = chrono::duration<double>(chrono::steady_clock::now() - start).count();
        printf("  k-means: %d points, %d clusters, init %.1f s\n", n, k, seconds);
    }

    for (int iteration = 1; iteration <= maxIterations; iteration++) {
        int changed = assignPoints(points);
        if (verbose) {
            double seconds = chrono::duration<double>(chrono::steady_clock::now() - start).count();
            printf("  iteration %3d  error %10.2f  changed %7d  (%.1f s)\n",
                   iteration, error, changed, seconds);
            fflush(stdout);
        }
        // Stop when (almost) nothing moves any more. We stop right after an
        // assignment, so every point is assigned to its nearest center.
        if (changed < n / 1000 + 1 || iteration == maxIterations) {
            break;
        }
        updateCenters(points);
    }
}

const vector<vector<float> >& KMeans::getCenters() const {
    return centers;
}

const vector<int>& KMeans::getAssignments() const {
    return assignments;
}

vector<int> KMeans::getClusterSizes() const {
    vector<int> sizes(k, 0);
    for (int i = 0; i < (int)assignments.size(); i++) {
        sizes[assignments[i]]++;
    }
    return sizes;
}

double KMeans::getError() const {
    return error;
}

// k-means++: the first center is a random point; every next center is a
// point chosen with probability proportional to its squared distance from
// the closest center chosen so far. Gives much better starting centers
// than k random points.
void KMeans::initPlusPlus(const vector<vector<float> >& points) {
    int n = (int)points.size();
    centers.clear();

    uniform_int_distribution<int> randomIndex(0, n - 1);
    centers.push_back(points[randomIndex(rng)]);

    // closest[i] = squared distance of point i to its closest center
    vector<double> closest(n);
    for (int i = 0; i < n; i++) {
        closest[i] = squaredDistance(points[i], centers[0]);
    }

    uniform_real_distribution<double> random01(0.0, 1.0);
    while ((int)centers.size() < k) {
        double total = 0;
        for (int i = 0; i < n; i++) {
            total += closest[i];
        }

        // walk until the running sum passes a random point in [0, total)
        double target = random01(rng) * total;
        int chosen = n - 1;
        double sum = 0;
        for (int i = 0; i < n; i++) {
            sum += closest[i];
            if (sum > target) {
                chosen = i;
                break;
            }
        }
        centers.push_back(points[chosen]);

        const vector<float>& newCenter = centers.back();
        for (int i = 0; i < n; i++) {
            double distance = squaredDistance(points[i], newCenter);
            if (distance < closest[i]) {
                closest[i] = distance;
            }
        }
    }
}

// Moves every point to its nearest center. Returns how many points changed
// cluster and updates the error.
int KMeans::assignPoints(const vector<vector<float> >& points) {
    int changed = 0;
    double total = 0;
    for (int i = 0; i < (int)points.size(); i++) {
        float distance;
        int best = nearestCenter(centers, points[i], distance);
        if (best != assignments[i]) {
            assignments[i] = best;
            changed++;
        }
        total += distance;
    }
    error = total / points.size();
    return changed;
}

// Every center becomes the mean of its points. An empty cluster gets a
// random point as its new center, so we always keep k clusters.
void KMeans::updateCenters(const vector<vector<float> >& points) {
    int dim = (int)points[0].size();
    vector<vector<double> > sums(k, vector<double>(dim, 0.0));
    vector<int> counts(k, 0);

    for (int i = 0; i < (int)points.size(); i++) {
        int c = assignments[i];
        counts[c]++;
        for (int j = 0; j < dim; j++) {
            sums[c][j] += points[i][j];
        }
    }

    uniform_int_distribution<int> randomIndex(0, (int)points.size() - 1);
    for (int c = 0; c < k; c++) {
        if (counts[c] == 0) {
            centers[c] = points[randomIndex(rng)];
            continue;
        }
        for (int j = 0; j < dim; j++) {
            centers[c][j] = (float)(sums[c][j] / counts[c]);
        }
    }
}
