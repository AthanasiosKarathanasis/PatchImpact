#pragma once

#include <filesystem>

namespace fs = std::filesystem;

class IndividualAssessmentAnalyzer
{
public:
    void analyze(const fs::path& targetPath);

private:
    bool isSourceFile(const fs::path& filePath);
    bool isExecutableFile(const fs::path& filePath);

    void analyzeSourceFile(const fs::path& filePath);
    void analyzeExecutableFile(const fs::path& filePath);
};
