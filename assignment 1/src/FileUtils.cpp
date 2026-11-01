#include "FileUtils.h"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>

using namespace std;

void makeDirectory(const string& path) {
    // EEXIST: the directory is already there, which is fine
    if (mkdir(path.c_str(), 0755) != 0 && errno != EEXIST) {
        fprintf(stderr, "Error: cannot create directory %s\n", path.c_str());
        exit(1);
    }
}

string tempFileName(const string& fileName) {
    return fileName + ".tmp";
}

void finishTempFile(const string& fileName) {
    string tempName = tempFileName(fileName);
    if (rename(tempName.c_str(), fileName.c_str()) != 0) {
        fprintf(stderr, "Error: cannot rename %s to %s\n", tempName.c_str(), fileName.c_str());
        exit(1);
    }
}
