#include "QualityImpactCalculator.h"

QualityImpact QualityImpactCalculator::calculate(
    double maintainabilityChange,
    int cppcheckChange,
    bool originalBuildSuccess,
    bool patchedBuildSuccess)
{
    QualityImpact result;

    result.maintainabilityChange = maintainabilityChange;
    result.cppcheckChange = cppcheckChange;

    result.buildStatusChanged =
        originalBuildSuccess != patchedBuildSuccess;

    result.patchedBuildFailed =
        originalBuildSuccess && !patchedBuildSuccess;

    result.qualityScore = 100.0;

    if (maintainabilityChange < 0.0)
    {
        result.qualityScore += maintainabilityChange * 2.0;
    }

    if (cppcheckChange > 0)
    {
        result.qualityScore -= cppcheckChange * 5.0;
    }

    if (result.patchedBuildFailed)
    {
        result.qualityScore -= 40.0;
    }
    else if (result.buildStatusChanged)
    {
        result.qualityScore -= 20.0;
    }

    if (result.qualityScore < 0.0)
    {
        result.qualityScore = 0.0;
    }

    if (result.qualityScore > 100.0)
    {
        result.qualityScore = 100.0;
    }

    return result;
}
