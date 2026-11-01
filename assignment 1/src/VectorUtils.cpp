#include "VectorUtils.h"

#include <cmath>

using namespace std;

float squaredDistance(const vector<float>& a, const vector<float>& b) {
    float sum = 0;
    int size = (int)a.size();
    for (int i = 0; i < size; i++) {
        float diff = a[i] - b[i];
        sum += diff * diff;
    }
    return sum;
}

double euclideanDistance(const vector<float>& a, const vector<float>& b) {
    return sqrt((double)squaredDistance(a, b));
}

int nearestCenter(const vector<vector<float> >& centers, const vector<float>& point,
                  float& bestDistance) {
    int best = 0;
    bestDistance = squaredDistance(centers[0], point);
    for (int c = 1; c < (int)centers.size(); c++) {
        float distance = squaredDistance(centers[c], point);
        if (distance < bestDistance) {
            bestDistance = distance;
            best = c;
        }
    }
    return best;
}
