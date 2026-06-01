#include "SourceFileCollector.h"

#include <algorithm>
#include <string>

using namespace std;

bool SourceFileCollector::isSourceFile(const fs::path& file)
{
    string ext = file.extension().string();

    return ext == ".c"   ||
           ext == ".cpp" ||
           ext == ".h"   ||
           ext == ".hpp";
}

bool SourceFileCollector::shouldIgnorePath(const fs::path& path)
{
    string p = path.string();

    return p.find("/build/") != string::npos ||
           p.find("/CMakeFiles/") != string::npos ||
           p.find("/.git/") != string::npos ||
           p.find("/.settings/") != string::npos;
}

vector<fs::path> SourceFileCollector::collect(const fs::path& folder)
{
    vector<fs::path> files;

    for (const auto& entry : fs::recursive_directory_iterator(folder))
    {
        if (shouldIgnorePath(entry.path()))
        {
            continue;
        }

        if (entry.is_regular_file() && isSourceFile(entry.path()))
        {
            files.push_back(fs::relative(entry.path(), folder));
        }
    }

    sort(files.begin(), files.end());

    return files;
}
