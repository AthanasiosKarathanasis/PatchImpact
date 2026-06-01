#include "OverallImpactCalculator.h"

/*OverallImpact OverallImpactCalculator::calculate(
    double structuralScore,
    double behavioralScore,
    double qualityScore)
 */
OverallImpact OverallImpactCalculator::calculate(
    double structuralScore,
    double qualityScore)


{
    OverallImpact result;
/*
    result.structuralScore = structuralScore;
    result.behavioralScore = behavioralScore;
    result.qualityScore = qualityScore;

    result.overallScore =
        structuralScore * 0.40 +
        behavioralScore * 0.30 +
        qualityScore * 0.30;
*/
    result.structuralScore = structuralScore;
    result.behavioralScore = -1.0;
    result.qualityScore = qualityScore;

    result.overallScore =
        structuralScore * 0.50 +
        qualityScore * 0.50;

    if (result.overallScore >= 90.0)
    {
        result.grade = "A";
        result.riskLevel = "Low";
    }
    else if (result.overallScore >= 80.0)
    {
        result.grade = "B";
        result.riskLevel = "Low";
    }
    else if (result.overallScore >= 70.0)
    {
        result.grade = "C";
        result.riskLevel = "Moderate";
    }
    else if (result.overallScore >= 60.0)
    {
        result.grade = "D";
        result.riskLevel = "High";
    }
    else
    {
        result.grade = "F";
        result.riskLevel = "Critical";
    }

    return result;
}
