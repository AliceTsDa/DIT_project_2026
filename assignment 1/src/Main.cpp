// Step 0: check that the build, OpenCV and SIFT work.
// Loads one image in grayscale and prints how many SIFT descriptors it has.

#include <cstdio>
#include <string>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/features2d.hpp>

using namespace std;

int main(int argc, char* argv[]) {
    string path = "datasets/hpatches-sequences-release/v_bird/1.ppm";
    if (argc > 1) {
        path = argv[1];
    }

    cv::Mat image = cv::imread(path, cv::IMREAD_GRAYSCALE);
    if (image.empty()) {
        fprintf(stderr, "Error: cannot read image %s\n", path.c_str());
        return 1;
    }
    printf("Image: %s (%d x %d)\n", path.c_str(), image.cols, image.rows);

    // cv::Ptr is OpenCV's own pointer type; SIFT::create() returns it
    cv::Ptr<cv::SIFT> sift = cv::SIFT::create();
    vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;
    sift->detectAndCompute(image, cv::noArray(), keypoints, descriptors);

    // descriptors: one row per keypoint, 128 float columns
    printf("SIFT descriptors: %d (dimension %d)\n", descriptors.rows, descriptors.cols);
    return 0;
}
