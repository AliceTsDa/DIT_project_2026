#pragma once

#include <string>
#include <vector>

// The SIFT descriptors of one image.
// OpenCV gives SIFT values as floats, but they are always whole numbers
// 0..255, so we keep them as unsigned char (4 times less memory and disk).
// All descriptors are stored one after the other in one vector:
// descriptor i is at data[i * DIM] ... data[i * DIM + DIM - 1].
class DescriptorSet {
public:
    static const int DIM = 128;

    DescriptorSet();

    int getCount() const;

    // Value j (0..127) of descriptor i
    int getValue(int i, int j) const;

    // Copies descriptor i into a float vector of size DIM
    std::vector<float> getDescriptor(int i) const;

    void clear();

    // Appends one descriptor (DIM values, each 0..255)
    void add(const std::vector<unsigned char>& descriptor);

    // Binary file: int count, then count * DIM bytes.
    // load() returns false if the file is missing or damaged.
    bool save(const std::string& fileName) const;
    bool load(const std::string& fileName);

private:
    int count;
    std::vector<unsigned char> data;
};
