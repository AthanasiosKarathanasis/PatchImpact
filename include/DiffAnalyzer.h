#pragma once

#include <filesystem>

namespace fs = std::filesystem;

struct DiffSummary
{
    int addedLines = 0;
    int removedLines = 0;
};

class DiffAnalyzer
{
public:
    bool filesAreDifferent(const fs::path& fileA,
                           const fs::path& fileB);

    DiffSummary analyzeDiff(const fs::path& originalFile,
                            const fs::path& patchedFile);
};
