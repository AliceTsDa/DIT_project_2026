#include "Parameters.h"

#include <cstdio>
#include <cstdlib>

using namespace std;

Parameters::Parameters() {
    hpPath = "datasets/hpatches-sequences-release";
    mirPath = "datasets/mirflickr";
    splitFile = "split.txt";
    outputFile = "output/output.txt";
    querySet = "validation";
    cacheDir = "cache";
    vocabSize = 256;
    maxSiftPerImage = 500;
    numMirflickr = 1000;
    seed = 1;

    method = "";

    lshK = 4;
    lshL = 5;
    lshW = 0.05;
    lshMaxCandidates = 0;

    cubeDim = 10;
    cubeW = 0.05;
    cubeMaxCandidates = 500;
    cubeProbes = 10;

    ivfClusters = 0;
    ivfProbe = 5;

    pqM = 16;
    pqBits = 8;
}

void Parameters::parse(int argc, char* argv[]) {
    // The default paths are relative to the program's folder, so that
    // e.g. "./code/search" also works from the assignment folder.
    // Paths given with flags stay relative to the current folder.
    useProgramFolder(argv[0]);

    // First pass: find the method, because -w and -M mean different
    // things for different methods.
    findMethod(argc, argv);

    // Second pass: every other flag is followed by a value.
    int i = 1;
    while (i < argc) {
        string flag = argv[i];
        if (isMethodFlag(flag)) {
            i++;
            continue;
        }
        if (i + 1 >= argc) {
            fprintf(stderr, "Error: missing value after %s\n", flag.c_str());
            exit(1);
        }
        parseFlag(flag, argv[i + 1]);
        i += 2;
    }

    validate();
}

void Parameters::useProgramFolder(const string& programPath) {
    // "./code/search" -> "./code/",  "./search" or "search" -> nothing to add
    size_t slash = programPath.rfind('/');
    if (slash == string::npos) {
        return;
    }
    string folder = programPath.substr(0, slash + 1);
    if (folder == "./") {
        return;
    }
    hpPath = folder + hpPath;
    mirPath = folder + mirPath;
    splitFile = folder + splitFile;
    outputFile = folder + outputFile;
    cacheDir = folder + cacheDir;
}

void Parameters::findMethod(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        string flag = argv[i];
        if (isMethodFlag(flag)) {
            if (method != "") {
                fprintf(stderr, "Error: give only one method (found -%s and %s)\n",
                        method.c_str(), flag.c_str());
                exit(1);
            }
            method = flag.substr(1);  // remove the leading '-'
        }
    }
    if (method == "") {
        fprintf(stderr, "Error: no method given (-exact, -lsh, -hypercube, -ivfflat or -ivfpq)\n");
        exit(1);
    }
}

void Parameters::parseFlag(const string& flag, const string& value) {
    // Common flags
    if (flag == "-hp") { hpPath = value; return; }
    if (flag == "-mir") { mirPath = value; return; }
    if (flag == "-split") { splitFile = value; return; }
    if (flag == "-o") { outputFile = value; return; }
    if (flag == "-set") { querySet = value; return; }
    if (flag == "-cache") { cacheDir = value; return; }
    if (flag == "-vocab") { vocabSize = toInt(flag, value); return; }
    if (flag == "-S") { maxSiftPerImage = toInt(flag, value); return; }
    if (flag == "-D") { numMirflickr = toInt(flag, value); return; }
    if (flag == "-seed") { seed = toInt(flag, value); return; }

    // Method flags: accepted only for the chosen method
    if (method == "lsh") {
        if (flag == "-k") { lshK = toInt(flag, value); return; }
        if (flag == "-L") { lshL = toInt(flag, value); return; }
        if (flag == "-w") { lshW = toDouble(flag, value); return; }
        if (flag == "-maxcand") { lshMaxCandidates = toInt(flag, value); return; }
    }
    if (method == "hypercube") {
        if (flag == "-kproj") { cubeDim = toInt(flag, value); return; }
        if (flag == "-w") { cubeW = toDouble(flag, value); return; }
        if (flag == "-M") { cubeMaxCandidates = toInt(flag, value); return; }
        if (flag == "-probes") { cubeProbes = toInt(flag, value); return; }
    }
    if (method == "ivfflat" || method == "ivfpq") {
        if (flag == "-kclusters") { ivfClusters = toInt(flag, value); return; }
        if (flag == "-nprobe") { ivfProbe = toInt(flag, value); return; }
    }
    if (method == "ivfpq") {
        if (flag == "-M") { pqM = toInt(flag, value); return; }
        if (flag == "-nbits") { pqBits = toInt(flag, value); return; }
    }

    fprintf(stderr, "Error: unknown flag %s for method -%s\n", flag.c_str(), method.c_str());
    exit(1);
}

void Parameters::validate() const {
    bool ok = true;
    if (querySet != "validation" && querySet != "test") {
        fprintf(stderr, "Error: -set must be validation or test\n");
        ok = false;
    }
    if (vocabSize < 1) { fprintf(stderr, "Error: -vocab must be >= 1\n"); ok = false; }
    if (maxSiftPerImage < 1) { fprintf(stderr, "Error: -S must be >= 1\n"); ok = false; }
    if (numMirflickr < 0) { fprintf(stderr, "Error: -D must be >= 0\n"); ok = false; }

    if (method == "lsh") {
        if (lshK < 1) { fprintf(stderr, "Error: -k must be >= 1\n"); ok = false; }
        if (lshL < 1) { fprintf(stderr, "Error: -L must be >= 1\n"); ok = false; }
        if (lshW <= 0) { fprintf(stderr, "Error: -w must be > 0\n"); ok = false; }
        if (lshMaxCandidates < 0) { fprintf(stderr, "Error: -maxcand must be >= 0\n"); ok = false; }
    }
    if (method == "hypercube") {
        if (cubeDim < 1 || cubeDim > 30) { fprintf(stderr, "Error: -kproj must be 1..30\n"); ok = false; }
        if (cubeW <= 0) { fprintf(stderr, "Error: -w must be > 0\n"); ok = false; }
        if (cubeMaxCandidates < 1) { fprintf(stderr, "Error: -M must be >= 1\n"); ok = false; }
        if (cubeProbes < 1) { fprintf(stderr, "Error: -probes must be >= 1\n"); ok = false; }
    }
    if (method == "ivfflat" || method == "ivfpq") {
        if (ivfClusters < 0) { fprintf(stderr, "Error: -kclusters must be >= 1\n"); ok = false; }
        if (ivfProbe < 1) { fprintf(stderr, "Error: -nprobe must be >= 1\n"); ok = false; }
    }
    if (method == "ivfpq") {
        if (pqM < 1) {
            fprintf(stderr, "Error: -M must be >= 1\n");
            ok = false;
        } else if (vocabSize % pqM != 0) {
            fprintf(stderr, "Error: -vocab (%d) must be divisible by -M (%d)\n", vocabSize, pqM);
            ok = false;
        }
        if (pqBits < 1 || pqBits > 16) { fprintf(stderr, "Error: -nbits must be 1..16\n"); ok = false; }
    }

    if (!ok) {
        exit(1);
    }
}

void Parameters::print() const {
    printf("Parameters:\n");
    printf("  -hp     %s\n", hpPath.c_str());
    printf("  -mir    %s\n", mirPath.c_str());
    printf("  -split  %s\n", splitFile.c_str());
    printf("  -o      %s\n", outputFile.c_str());
    printf("  -set    %s\n", querySet.c_str());
    printf("  -cache  %s\n", cacheDir.c_str());
    printf("  -vocab  %d\n", vocabSize);
    printf("  -S      %d\n", maxSiftPerImage);
    printf("  -D      %d\n", numMirflickr);
    printf("  -seed   %d\n", seed);
    printf("  method  %s\n", method.c_str());

    if (method == "lsh") {
        printf("  -k      %d\n", lshK);
        printf("  -L      %d\n", lshL);
        printf("  -w      %g\n", lshW);
        if (lshMaxCandidates == 0) {
            printf("  -maxcand no limit\n");
        } else {
            printf("  -maxcand %d\n", lshMaxCandidates);
        }
    }
    if (method == "hypercube") {
        printf("  -kproj  %d\n", cubeDim);
        printf("  -w      %g\n", cubeW);
        printf("  -M      %d\n", cubeMaxCandidates);
        printf("  -probes %d\n", cubeProbes);
    }
    if (method == "ivfflat" || method == "ivfpq") {
        if (ivfClusters == 0) {
            printf("  -kclusters ceil(sqrt(n))\n");
        } else {
            printf("  -kclusters %d\n", ivfClusters);
        }
        printf("  -nprobe %d\n", ivfProbe);
    }
    if (method == "ivfpq") {
        printf("  -M      %d\n", pqM);
        printf("  -nbits  %d\n", pqBits);
    }
}

void Parameters::printUsage(const char* programName) {
    printf("Usage: %s [common flags] <method> [method flags]\n", programName);
    printf("Common flags:\n");
    printf("  -hp <path> -mir <path> -split <file> -o <file> -set <validation|test>\n");
    printf("  -vocab <int> -S <int> -D <int> -seed <int> -cache <dir>\n");
    printf("Methods and their flags:\n");
    printf("  -exact\n");
    printf("  -lsh       -k <int> -L <int> -w <double> [-maxcand <int>]\n");
    printf("  -hypercube -kproj <int> -w <double> -M <int> -probes <int>\n");
    printf("  -ivfflat   -kclusters <int> -nprobe <int>\n");
    printf("  -ivfpq     -kclusters <int> -nprobe <int> -M <int> -nbits <int>\n");
}

int Parameters::toInt(const string& flag, const string& value) {
    char* end;
    long result = strtol(value.c_str(), &end, 10);
    if (value.empty() || *end != '\0') {
        fprintf(stderr, "Error: %s expects an integer, got '%s'\n", flag.c_str(), value.c_str());
        exit(1);
    }
    return (int)result;
}

double Parameters::toDouble(const string& flag, const string& value) {
    char* end;
    double result = strtod(value.c_str(), &end);
    if (value.empty() || *end != '\0') {
        fprintf(stderr, "Error: %s expects a number, got '%s'\n", flag.c_str(), value.c_str());
        exit(1);
    }
    return result;
}

bool Parameters::isMethodFlag(const string& flag) {
    return flag == "-exact" || flag == "-lsh" || flag == "-hypercube" ||
           flag == "-ivfflat" || flag == "-ivfpq";
}
