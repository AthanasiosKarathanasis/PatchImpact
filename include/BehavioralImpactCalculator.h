#pragma once

#include "DynamicMeasurementsAnalyzer.h"

#include <string>

struct BehavioralImpact
{
    double elapsedChangePercent = 0.0;
    double cpuChangePercent = 0.0;
    double memoryChangePercent = 0.0;
    double binarySizeChangePercent = 0.0;

    bool exitStatusChanged = false;

    double behavioralScore = 100.0;

    std::string grade;
    std::string riskLevel;
};

class BehavioralImpactCalculator
{
public:
    BehavioralImpact calculate(
        const DynamicMeasurements& original,
        const DynamicMeasurements& patched);
};
