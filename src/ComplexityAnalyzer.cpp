#include "ComplexityAnalyzer.h"

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

int ComplexityAnalyzer::calculateComplexity(const fs::path& file)
{
    string command = "pmccabe " + quotePath(file);
    string output = runCommand(command);

    int totalComplexity = 0;
    string line;
    stringstream outputStream(output);

    while (getline(outputStream, line))
    {
        stringstream lineStream(line);
        int complexityValue = 0;

        if (lineStream >> complexityValue)
        {
            totalComplexity += complexityValue;
        }
    }

    return totalComplexity;
}

int ComplexityAnalyzer::countFunctions(const fs::path& file)
{
    string command = "pmccabe " + quotePath(file);
    string output = runCommand(command);

    int functionCount = 0;
    string line;
    stringstream outputStream(output);

    while (getline(outputStream, line))
    {
        if (!line.empty())
        {
            functionCount++;
        }
    }

    return functionCount;
}
