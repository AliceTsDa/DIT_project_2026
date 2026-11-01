#pragma once

#include <string>

// All command line parameters of the program.
// The fields are public: the class is a simple "bag" of settings that the
// rest of the program only reads after parse() has filled it.
class Parameters {
public:
    // Common parameters
    std::string hpPath;      // -hp
    std::string mirPath;     // -mir
    std::string splitFile;   // -split
    std::string outputFile;  // -o
    std::string querySet;    // -set  ("validation" or "test")
    std::string cacheDir;    // -cache (our extra flag)
    int vocabSize;           // -vocab (K)
    int maxSiftPerImage;     // -S
    int numMirflickr;        // -D
    int seed;                // -seed

    // Chosen method: "exact", "lsh", "hypercube", "ivfflat" or "ivfpq"
    std::string method;

    // LSH
    int lshK;                // -k
    int lshL;                // -L
    double lshW;             // -w

    // Hypercube
    int cubeDim;             // -kproj
    double cubeW;            // -w
    int cubeMaxCandidates;   // -M
    int cubeProbes;          // -probes

    // IVFFlat and IVFPQ
    int ivfClusters;         // -kclusters (0 means: use ceil(sqrt(n)))
    int ivfProbe;            // -nprobe

    // IVFPQ only
    int pqM;                 // -M
    int pqBits;              // -nbits

    // Sets all default values
    Parameters();

    // Reads argv. On any error prints a message and exits the program.
    void parse(int argc, char* argv[]);

    // Prints all parameters (only the ones of the chosen method)
    void print() const;

    // Prints how to run the program
    static void printUsage(const char* programName);

private:
    void useProgramFolder(const std::string& programPath);
    void findMethod(int argc, char* argv[]);
    void parseFlag(const std::string& flag, const std::string& value);
    void validate() const;

    static int toInt(const std::string& flag, const std::string& value);
    static double toDouble(const std::string& flag, const std::string& value);
    static bool isMethodFlag(const std::string& flag);
};
