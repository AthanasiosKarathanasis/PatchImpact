#pragma once

struct QualityImpact
{
    double maintainabilityChange = 0.0;
    int cppcheckChange = 0;

    bool buildStatusChanged = false;
    bool patchedBuildFailed = false;

    double qualityScore = 100.0;
};

class QualityImpactCalculator
{
public:
    QualityImpact calculate(
        double maintainabilityChange,
        int cppcheckChange,
        bool originalBuildSuccess,
        bool patchedBuildSuccess);
};
