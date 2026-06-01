#include "CppcheckAnalyzer.h"

#include <cstdio>
#include <array>
#include <sstream>
#include <string>

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
        return "ERROR: Failed to run command.";
    }

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
    {
        result += buffer.data();
    }

    pclose(pipe);
    return result;
}

CppcheckResults CppcheckAnalyzer::analyze(const fs::path& file)
{
    CppcheckResults results;

    string language;
    string ext = file.extension().string();

    if (ext == ".c")
    {
        language = "c";
    }
    else
    {
        language = "c++";
    }

    string command = "cppcheck --language="
                   + language
                   + " --enable=all --quiet "
                   + quotePath(file)
                   + " 2>&1";

    string output = runCommand(command);

    string line;
    stringstream outputStream(output);

    while (getline(outputStream, line))
    {
        if (line.find("[checkersReport]") != string::npos)
        {
            continue;
        }

        if (line.find("error:") != string::npos)
        {
            results.errors++;
        }
        else if (line.find("warning:") != string::npos)
        {
            results.warnings++;
        }
        else if (line.find("style:") != string::npos)
        {
            results.style++;
        }
        else if (line.find("information:") != string::npos)
        {
            results.information++;
        }
    }

    return results;
}
