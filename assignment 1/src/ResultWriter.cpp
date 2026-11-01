#include "ResultWriter.h"

#include <cstdio>
#include <cstdlib>

#include "FileUtils.h"
#include "MetricsSummary.h"
#include "QueryResult.h"

using namespace std;

void ResultWriter::write(const string& fileName, const string& methodName,
                         const ImageDatabase& database, const Evaluator& evaluator) {
    makeFolderOf(fileName);  // e.g. creates "output/" for "output/exact.txt"
    FILE* file = fopen(fileName.c_str(), "w");
    if (file == NULL) {
        fprintf(stderr, "Error: cannot write output file %s\n", fileName.c_str());
        exit(1);
    }

    fprintf(file, "%s\n", methodName.c_str());

    for (const auto& result : evaluator.getResults()) {
        fprintf(file, "Query: %s\n", result.sequence.c_str());
        for (int rank = 1; rank <= 10; rank++) {
            if (rank <= (int)result.ranking.size()) {
                int index = result.ranking[rank - 1];
                fprintf(file, "Rank-%d: %s\n", rank, database.getImageId(index).c_str());
            } else {
                fprintf(file, "Rank-%d: -\n", rank);
            }
        }
        fprintf(file, "Recall@5: %.6f\n", result.recall5);
        fprintf(file, "Recall@10: %.6f\n", result.recall10);
        fprintf(file, "AP@10: %.6f\n", result.ap10);
        fprintf(file, "\n");
    }

    MetricsSummary all = evaluator.getSummary("all");
    fprintf(file, "Mean Recall@5: %.6f\n", all.meanRecall5);
    fprintf(file, "Mean Recall@10: %.6f\n", all.meanRecall10);
    fprintf(file, "mAP@10: %.6f\n", all.map10);
    fprintf(file, "Average query time: %.6f\n", all.averageTimeMs);

    fclose(file);
}
