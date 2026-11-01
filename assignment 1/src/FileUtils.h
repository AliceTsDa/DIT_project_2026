#pragma once

#include <string>

// Free helper functions for files and directories.

// Creates a directory if it does not exist yet. Exits on error.
void makeDirectory(const std::string& path);

// Creates the folder a file goes into, e.g. "output" for "output/exact.txt"
// (only the last folder; the ones above it must exist). Exits on error.
void makeFolderOf(const std::string& fileName);

// Safe writing of cache files: the caller first writes "<fileName>.tmp",
// then calls this to rename it to "<fileName>". A run stopped in the middle
// (Ctrl+C) then never leaves a half-written file. Exits on error.
std::string tempFileName(const std::string& fileName);
void finishTempFile(const std::string& fileName);
