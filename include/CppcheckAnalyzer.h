#pragma once

#include <filesystem>

namespace fs = std::filesystem;

struct CppcheckResults
{
    int errors = 0;
    int warnings = 0;
    int style = 0;
    int information = 0;
};

class CppcheckAnalyzer
{
public:
    CppcheckResults analyze(const fs::path& file);
};
