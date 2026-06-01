#pragma once

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

struct LevenshteinResults
{
    int distance = 0;
    double similarityPercent = 0.0;
};

class LevenshteinAnalyzer
{
public:
    LevenshteinResults analyze(const fs::path& originalFile,
                               const fs::path& patchedFile);

private:
    std::string readFile(const fs::path& file);
    int calculateDistance(const std::string& first,
                          const std::string& second);
};
