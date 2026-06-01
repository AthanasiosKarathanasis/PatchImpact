#pragma once

#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

class CsvExportManager
{
private:
    std::ofstream file;
    bool enabled = false;

    CsvExportManager() = default;

public:
    static CsvExportManager& instance();

    bool open(const fs::path& outputPath);
    void close();

    bool isEnabled() const;

    void writeRawLine(const std::string& line);

    void writeIndividualHeader();

    void writeIndividualSourceRow(
        const fs::path& path,
        long fileSizeBytes,
        int complexity,
        int functionCount,
        int sloc,
        int commentLines,
        int blankLines,
        double maintainability,
        int cppcheckErrors,
        int cppcheckWarnings,
        int cppcheckStyle,
        int cppcheckInformation,
        int cppcheckTotal);

    void writeIndividualExecutableRow(
        const fs::path& path,
        long binarySizeBytes,
        double elapsedSeconds,
        double userCpuSeconds,
        double systemCpuSeconds,
        long peakMemoryKb,
        int exitStatus);


    void writeMassHeader();

    void writeMassSourcePairRow(
        const fs::path& originalPath,
        const fs::path& patchedPath,
        int originalComplexity,
        int patchedComplexity,
        int complexityChange,
        int originalFunctions,
        int patchedFunctions,
        int functionChange,
        int originalSloc,
        int patchedSloc,
        int slocChange,
        double similarityPercent,
        double maintainabilityChange,
        int cppcheckChange,
        double structuralScore,
        double qualityScore);

    void writeMassExecutablePairRow(
        const fs::path& originalPath,
        const fs::path& patchedPath,
        double originalElapsed,
        double patchedElapsed,
        double originalCpuTotal,
        double patchedCpuTotal,
        long originalMemoryKb,
        long patchedMemoryKb,
        long originalBinaryBytes,
        long patchedBinaryBytes,
        int originalExitStatus,
        int patchedExitStatus,
        double elapsedChangePercent,
        double cpuChangePercent,
        double memoryChangePercent,
        double binaryChangePercent,
        bool exitStatusChanged,
        double dynamicScore,
        const std::string& grade,
        const std::string& riskLevel);

private:
    std::string escape(const std::string& value);
};
