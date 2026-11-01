#include "SiftExtractor.h"

#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

using namespace std;

SiftExtractor::SiftExtractor() {
    sift = cv::SIFT::create();
}

bool SiftExtractor::extract(const string& imagePath, DescriptorSet& result) {
    result.clear();

    cv::Mat image = cv::imread(imagePath, cv::IMREAD_GRAYSCALE);
    if (image.empty()) {
        return false;
    }

    vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;  // one row per keypoint, 128 float columns
    sift->detectAndCompute(image, cv::noArray(), keypoints, descriptors);

    vector<unsigned char> one(DescriptorSet::DIM);
    for (int i = 0; i < descriptors.rows; i++) {
        for (int j = 0; j < DescriptorSet::DIM; j++) {
            // values are whole numbers 0..255; round just to be safe
            float value = descriptors.at<float>(i, j);
            one[j] = (unsigned char)(value + 0.5f);
        }
        result.add(one);
    }
    return true;
}
