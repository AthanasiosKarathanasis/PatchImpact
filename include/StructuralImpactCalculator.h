#pragma once

struct StructuralImpact
{
    double complexityChangePercent = 0.0;
    double functionChangePercent = 0.0;
    double slocChangePercent = 0.0;
    double levenshteinSimilarityPercent = 100.0;

    double structuralScore = 100.0;
};

class StructuralImpactCalculator
{
public:
    StructuralImpact calculate(
        int originalComplexity,
        int patchedComplexity,
        int originalFunctions,
        int patchedFunctions,
        int originalSloc,
        int patchedSloc,
        double similarityPercent);
};
