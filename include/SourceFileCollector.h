#pragma once

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

class SourceFileCollector
{
public:
    std::vector<fs::path> collect(const fs::path& folder);

private:
    bool isSourceFile(const fs::path& file);
    bool shouldIgnorePath(const fs::path& path);
};
