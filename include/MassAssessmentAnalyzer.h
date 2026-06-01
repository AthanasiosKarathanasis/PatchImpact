#pragma once

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

class MassAssessmentAnalyzer
{
public:
    void analyze(const fs::path& folderPath);

private:
    bool isSourceFile(const fs::path& filePath);
    bool isExecutableFile(const fs::path& filePath);

    std::vector<fs::path> collectSourceFiles(const fs::path& folderPath);
    std::vector<fs::path> collectExecutableFiles(const fs::path& folderPath);

    void assessSourcePair(const fs::path& originalFile,
                          const fs::path& patchedFile);

    void assessExecutablePair(const fs::path& originalExecutable,
                              const fs::path& patchedExecutable);
};
