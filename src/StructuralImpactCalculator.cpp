#include "StructuralImpactCalculator.h"

StructuralImpact StructuralImpactCalculator::calculate(
    int originalComplexity,
    int patchedComplexity,
    int originalFunctions,
    int patchedFunctions,
    int originalSloc,
    int patchedSloc,
    double similarityPercent)
{
    StructuralImpact result;

    if (originalComplexity > 0)
    {
        result.complexityChangePercent =
            ((double)(patchedComplexity - originalComplexity)
            / originalComplexity) * 100.0;
    }

    if (originalFunctions > 0)
    {
        result.functionChangePercent =
            ((double)(patchedFunctions - originalFunctions)
            / originalFunctions) * 100.0;
    }

    if (originalSloc > 0)
    {
        result.slocChangePercent =
            ((double)(patchedSloc - originalSloc)
            / originalSloc) * 100.0;
    }

    result.levenshteinSimilarityPercent = similarityPercent;

    result.structuralScore = 100.0;

    if (result.complexityChangePercent > 0.0)
    {
        result.structuralScore -=
            result.complexityChangePercent * 0.4;
    }

    if (result.functionChangePercent > 0.0)
    {
        result.structuralScore -=
            result.functionChangePercent * 0.2;
    }

    if (result.slocChangePercent > 0.0)
    {
        result.structuralScore -=
            result.slocChangePercent * 0.2;
    }

    result.structuralScore -=
        (100.0 - result.levenshteinSimilarityPercent) * 0.2;

    if (result.structuralScore < 0.0)
    {
        result.structuralScore = 0.0;
    }

    if (result.structuralScore > 100.0)
    {
        result.structuralScore = 100.0;
    }

    return result;
}
