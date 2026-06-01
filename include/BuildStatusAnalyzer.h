#pragma once

#include <filesystem>

namespace fs = std::filesystem;

struct BuildStatus
{
    bool success = false;
    int exitCode = -1;
};

class BuildStatusAnalyzer
{
public:
    BuildStatus analyze(const fs::path& folder);
};
