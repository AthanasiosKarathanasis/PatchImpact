#include "BuildStatusAnalyzer.h"

BuildStatus BuildStatusAnalyzer::analyze(const fs::path& folder)
{
    BuildStatus result;

    if (fs::exists(folder / "CMakeLists.txt"))
    {
        result.success = true;
        result.exitCode = 0;
        return result;
    }

    if (fs::exists(folder / "Makefile") ||
        fs::exists(folder / "makefile"))
    {
        result.success = true;
        result.exitCode = 0;
        return result;
    }

    result.success = false;
    result.exitCode = -1;

    return result;
}
