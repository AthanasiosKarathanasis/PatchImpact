#pragma once

#include <filesystem>

namespace fs = std::filesystem;

struct DynamicMeasurements
{
    bool available = false;

    double elapsedSeconds = 0.0;
    double userCpuSeconds = 0.0;
    double systemCpuSeconds = 0.0;
    long binarySizeBytes = 0;

    long peakMemoryKb = 0;

    int exitStatus = -1;
};

class DynamicMeasurementsAnalyzer
{
public:
    DynamicMeasurements analyze(const fs::path& executablePath);
};
