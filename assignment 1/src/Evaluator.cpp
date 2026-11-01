#include "Evaluator.h"

#include <algorithm>

#include "Split.h"

using namespace std;

Evaluator::Evaluator(const ImageDatabase& database) : database(database) {
}

void Evaluator::addQuery(int q, const vector<int>& ranking, double timeMs) {
    QueryResult result;
    result.sequence = database.getQuerySequence(q);
    result.ranking = ranking;
    result.recall5 = recallAt(q, ranking, 5);
    result.recall10 = recallAt(q, ranking, 10);
    result.ap10 = averagePrecisionAt10(q, ranking);
    result.timeMs = timeMs;
    results.push_back(result);
}

const vector<QueryResult>& Evaluator::getResults() const {
    return results;
}

MetricsSummary Evaluator::getSummary(const string& group) const {
    MetricsSummary summary;
    summary.numQueries = 0;
    summary.meanRecall5 = 0;
    summary.meanRecall10 = 0;
    summary.map10 = 0;
    summary.averageTimeMs = 0;
    summary.fewerThan10 = 0;

    for (const auto& result : results) {
        bool illumination = Split::isIllumination(result.sequence);
        if (group == "illumination" && !illumination) {
            continue;
        }
        if (group == "viewpoint" && illumination) {
            continue;
        }
        summary.numQueries++;
        summary.meanRecall5 += result.recall5;
        summary.meanRecall10 += result.recall10;
        summary.map10 += result.ap10;
        summary.averageTimeMs += result.timeMs;
        if ((int)result.ranking.size() < 10) {
            summary.fewerThan10++;
        }
    }

    if (summary.numQueries > 0) {
        summary.meanRecall5 /= summary.numQueries;
        summary.meanRecall10 /= summary.numQueries;
        summary.map10 /= summary.numQueries;
        summary.averageTimeMs /= summary.numQueries;
    }
    return summary;
}

double Evaluator::recallAt(int q, const vector<int>& ranking, int n) const {
    int found = 0;
    int limit = min(n, (int)ranking.size());
    for (int i = 0; i < limit; i++) {
        if (database.isRelevant(q, ranking[i])) {
            found++;
        }
    }
    return (double)found / NUM_RELEVANT;
}

double Evaluator::averagePrecisionAt10(int q, const vector<int>& ranking) const {
    double sum = 0;
    int found = 0;  // j: how many relevant results so far
    int limit = min(10, (int)ranking.size());
    for (int i = 0; i < limit; i++) {
        if (database.isRelevant(q, ranking[i])) {
            found++;
            int position = i + 1;  // r_j, positions start at 1
            sum += (double)found / position;
        }
    }
    return sum / NUM_RELEVANT;
}
