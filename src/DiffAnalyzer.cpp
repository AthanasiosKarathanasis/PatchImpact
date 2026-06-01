#include "DiffAnalyzer.h"

#include <fstream>
#include <string>
#include <sstream>
#include <array>
#include <cstdio>

using namespace std;

static string quotePath(const fs::path& path)
{
    return "'" + path.string() + "'";
}

static string runCommand(const string& command)
{
    string result;
    array<char, 256> buffer;

    FILE* pipe = popen(command.c_str(), "r");

    if (!pipe)
    {
        return "";
    }

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
    {
        result += buffer.data();
    }

    pclose(pipe);
    return result;
}

bool DiffAnalyzer::filesAreDifferent(const fs::path& fileA,
                                     const fs::path& fileB)
{
    if (fs::file_size(fileA) != fs::file_size(fileB))
    {
        return true;
    }

    ifstream a(fileA, ios::binary);
    ifstream b(fileB, ios::binary);

    if (!a || !b)
    {
        return true;
    }

    char charA;
    char charB;

    while (a.get(charA) && b.get(charB))
    {
        if (charA != charB)
        {
            return true;
        }
    }

    return false;
}

DiffSummary DiffAnalyzer::analyzeDiff(const fs::path& originalFile,
                                      const fs::path& patchedFile)
{
    DiffSummary summary;

    string command =
        "diff -u "
        + quotePath(originalFile)
        + " "
        + quotePath(patchedFile);

    string diffOutput = runCommand(command);

    string line;
    stringstream diffStream(diffOutput);

    while (getline(diffStream, line))
    {
        if (line.rfind("+++", 0) == 0 ||
            line.rfind("---", 0) == 0 ||
            line.rfind("@@", 0) == 0)
        {
            continue;
        }

        if (!line.empty() && line[0] == '+')
        {
            summary.addedLines++;
        }
        else if (!line.empty() && line[0] == '-')
        {
            summary.removedLines++;
        }
    }

    return summary;
}
