#pragma once

// Mean metrics over a group of queries (all, illumination or viewpoint)
struct MetricsSummary {
    int numQueries;
    double meanRecall5;
    double meanRecall10;
    double map10;
    double averageTimeMs;
    int fewerThan10;  // queries that got fewer than 10 results
};
