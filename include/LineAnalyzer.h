#pragma once

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

struct LineMetrics
{
    int codeLines = 0;
    int commentLines = 0;
    int blankLines = 0;
};

class LineAnalyzer
{
public:
    LineMetrics analyze(const fs::path& file);

private:
    std::string trim(const std::string& text);
};
