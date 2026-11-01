#include "Split.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <set>

using namespace std;

Split::Split(const string& fileName) {
    ifstream file(fileName);
    if (!file) {
        fprintf(stderr, "Error: cannot open split file %s\n", fileName.c_str());
        exit(1);
    }

    set<string> seen;  // to catch a sequence that appears twice
    string sequence;
    string part;
    while (file >> sequence >> part) {
        if (seen.count(sequence) > 0) {
            fprintf(stderr, "Error: sequence %s appears twice in %s\n", sequence.c_str(), fileName.c_str());
            exit(1);
        }
        if (sequence.substr(0, 2) != "i_" && sequence.substr(0, 2) != "v_") {
            fprintf(stderr, "Error: sequence %s must start with i_ or v_\n", sequence.c_str());
            exit(1);
        }
        seen.insert(sequence);

        if (part == "train") {
            train.push_back(sequence);
        } else if (part == "validation") {
            validation.push_back(sequence);
        } else if (part == "test") {
            test.push_back(sequence);
        } else {
            fprintf(stderr, "Error: unknown part '%s' for sequence %s\n", part.c_str(), sequence.c_str());
            exit(1);
        }
    }

    if (train.empty() || validation.empty() || test.empty()) {
        fprintf(stderr, "Error: split file %s must have train, validation and test sequences\n", fileName.c_str());
        exit(1);
    }
}

const vector<string>& Split::getSequences(const string& part) const {
    if (part == "train") {
        return train;
    }
    if (part == "validation") {
        return validation;
    }
    if (part == "test") {
        return test;
    }
    fprintf(stderr, "Error: unknown split part '%s'\n", part.c_str());
    exit(1);
}

int Split::countIllumination(const string& part) const {
    int count = 0;
    for (const auto& sequence : getSequences(part)) {
        if (isIllumination(sequence)) {
            count++;
        }
    }
    return count;
}

int Split::countViewpoint(const string& part) const {
    return (int)getSequences(part).size() - countIllumination(part);
}

bool Split::isIllumination(const string& sequence) {
    return sequence.substr(0, 2) == "i_";
}
