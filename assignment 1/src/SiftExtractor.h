#pragma once

#include <string>
#include <opencv2/features2d.hpp>

#include "DescriptorSet.h"

// Reads an image in grayscale with OpenCV and extracts its SIFT descriptors.
// The same code is used for HPatches (.ppm) and MIRFlickr (.jpg).
class SiftExtractor {
public:
    SiftExtractor();

    // Fills "result" with all SIFT descriptors of the image.
    // Returns false if the image cannot be read.
    bool extract(const std::string& imagePath, DescriptorSet& result);

private:
    // cv::Ptr is OpenCV's own pointer type; SIFT::create() returns it
    cv::Ptr<cv::SIFT> sift;
};
