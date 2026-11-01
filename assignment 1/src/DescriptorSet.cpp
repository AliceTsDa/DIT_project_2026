#include "DescriptorSet.h"

#include <cstdio>

using namespace std;

DescriptorSet::DescriptorSet() {
    count = 0;
}

int DescriptorSet::getCount() const {
    return count;
}

int DescriptorSet::getValue(int i, int j) const {
    return data[i * DIM + j];
}

vector<float> DescriptorSet::getDescriptor(int i) const {
    vector<float> result(DIM);
    copyDescriptor(i, result);
    return result;
}

void DescriptorSet::copyDescriptor(int i, vector<float>& result) const {
    result.resize(DIM);
    for (int j = 0; j < DIM; j++) {
        result[j] = data[i * DIM + j];
    }
}

void DescriptorSet::clear() {
    count = 0;
    data.clear();
}

void DescriptorSet::add(const vector<unsigned char>& descriptor) {
    for (int j = 0; j < DIM; j++) {
        data.push_back(descriptor[j]);
    }
    count++;
}

bool DescriptorSet::save(const string& fileName) const {
    FILE* file = fopen(fileName.c_str(), "wb");
    if (file == NULL) {
        return false;
    }
    bool ok = fwrite(&count, sizeof(int), 1, file) == 1;
    if (ok && count > 0) {
        ok = (int)fwrite(data.data(), 1, data.size(), file) == count * DIM;
    }
    fclose(file);
    return ok;
}

bool DescriptorSet::load(const string& fileName) {
    clear();
    FILE* file = fopen(fileName.c_str(), "rb");
    if (file == NULL) {
        return false;
    }

    int fileCount = 0;
    // a sane image has far fewer than a million descriptors
    bool ok = fread(&fileCount, sizeof(int), 1, file) == 1 && fileCount >= 0 && fileCount < 1000000;
    if (ok && fileCount > 0) {
        data.resize(fileCount * DIM);
        ok = (int)fread(data.data(), 1, data.size(), file) == fileCount * DIM;
    }
    fclose(file);

    if (!ok) {
        clear();
        return false;
    }
    count = fileCount;
    return true;
}
