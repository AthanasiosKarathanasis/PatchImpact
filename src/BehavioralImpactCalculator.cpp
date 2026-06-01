#include "BehavioralImpactCalculator.h"

static double percentChange(double originalValue, double patchedValue)
{
    if (originalValue <= 0.0)
    {
        return 0.0;
    }

    return ((patchedValue - originalValue) / originalValue) * 100.0;
}

static void assignGradeAndRisk(BehavioralImpact& result)
{
    if (result.behavioralScore >= 90.0)
    {
        result.grade = "A";
        result.riskLevel = "Low";
    }
    else if (result.behavioralScore >= 80.0)
    {
        result.grade = "B";
        result.riskLevel = "Low";
    }
    else if (result.behavioralScore >= 70.0)
    {
        result.grade = "C";
        result.riskLevel = "Moderate";
    }
    else if (result.behavioralScore >= 60.0)
    {
        result.grade = "D";
        result.riskLevel = "High";
    }
    else
    {
        result.grade = "F";
        result.riskLevel = "Critical";
    }
}

BehavioralImpact BehavioralImpactCalculator::calculate(
    const DynamicMeasurements& original,
    const DynamicMeasurements& patched)
{
    BehavioralImpact result;

    double originalCpu =
        original.userCpuSeconds + original.systemCpuSeconds;

    double patchedCpu =
        patched.userCpuSeconds + patched.systemCpuSeconds;

    result.elapsedChangePercent =
        percentChange(original.elapsedSeconds, patched.elapsedSeconds);

    result.cpuChangePercent =
        percentChange(originalCpu, patchedCpu);

    result.memoryChangePercent =
        percentChange(
            static_cast<double>(original.peakMemoryKb),
            static_cast<double>(patched.peakMemoryKb));

    result.binarySizeChangePercent =
        percentChange(
            static_cast<double>(original.binarySizeBytes),
            static_cast<double>(patched.binarySizeBytes));

    result.exitStatusChanged =
        original.exitStatus != patched.exitStatus;

    result.behavioralScore = 100.0;

    if (result.elapsedChangePercent > 0.0)
    {
        result.behavioralScore -= result.elapsedChangePercent * 0.30;
    }

    if (result.cpuChangePercent > 0.0)
    {
        result.behavioralScore -= result.cpuChangePercent * 0.30;
    }

    if (result.memoryChangePercent > 0.0)
    {
        result.behavioralScore -= result.memoryChangePercent * 0.20;
    }

    if (result.binarySizeChangePercent > 0.0)
    {
        result.behavioralScore -= result.binarySizeChangePercent * 0.10;
    }

    if (result.exitStatusChanged)
    {
        result.behavioralScore -= 30.0;
    }

    if (result.behavioralScore < 0.0)
    {
        result.behavioralScore = 0.0;
    }

    if (result.behavioralScore > 100.0)
    {
        result.behavioralScore = 100.0;
    }

    assignGradeAndRisk(result);

    return result;
}
