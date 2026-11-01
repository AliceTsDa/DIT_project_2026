#pragma once

#include <string>
#include <vector>

// The train / validation / test split of the HPatches sequences,
// read from a text file with lines like "v_bird validation".
class Split {
public:
    // Reads the split file. On any error prints a message and exits.
    Split(const std::string& fileName);

    // Sequence names of one part: "train", "validation" or "test"
    const std::vector<std::string>& getSequences(const std::string& part) const;

    // How many sequences of a part are illumination (i_) / viewpoint (v_)
    int countIllumination(const std::string& part) const;
    int countViewpoint(const std::string& part) const;

    // Illumination sequences start with "i_", viewpoint with "v_"
    static bool isIllumination(const std::string& sequence);

private:
    std::vector<std::string> train;
    std::vector<std::string> validation;
    std::vector<std::string> test;
};
