#pragma once

#include <filesystem>

namespace fs = std::filesystem;

class ComplexityAnalyzer
{
public:
    int calculateComplexity(const fs::path& file);
    int countFunctions(const fs::path& file);
};
