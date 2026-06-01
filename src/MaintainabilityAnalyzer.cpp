#include "MaintainabilityAnalyzer.h"

double MaintainabilityAnalyzer::calculate(
    int complexity,
    int sloc,
    int comments,
    int cppcheckFindings)
{
    double score = 100.0;

    score -= complexity * 0.5;

    score -= sloc * 0.02;

    score -= cppcheckFindings * 2.0;

    score += comments * 0.10;

    if (score < 0.0)
    {
        score = 0.0;
    }

    if (score > 100.0)
    {
        score = 100.0;
    }

    return score;
}
