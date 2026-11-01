#pragma once

#include <string>

#include "Evaluator.h"
#include "ImageDatabase.h"

// Writes the output file in the format of the assignment:
//
//   <METHOD NAME>
//   Query: <sequence_id>
//   Rank-1: <image_id>
//   ...
//   Rank-10: <image_id>
//   Recall@5: <double>
//   Recall@10: <double>
//   AP@10: <double>
//   (one block per query)
//   Mean Recall@5: <double>
//   Mean Recall@10: <double>
//   mAP@10: <double>
//   Average query time: <double>    (milliseconds)
//
// Missing ranks (fewer than 10 results) are written as "Rank-k: -".
class ResultWriter {
public:
    // Exits with an error if the file cannot be written
    static void write(const std::string& fileName, const std::string& methodName,
                      const ImageDatabase& database, const Evaluator& evaluator);
};
