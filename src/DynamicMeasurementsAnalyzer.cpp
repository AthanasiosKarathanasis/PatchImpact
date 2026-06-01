#include "DynamicMeasurementsAnalyzer.h"

#include <cstdio>
#include <sstream>
#include <string>
#include <chrono>

using namespace std;

DynamicMeasurements DynamicMeasurementsAnalyzer::analyze(
    const fs::path& executablePath)
{
    DynamicMeasurements result;

    result.available =
        fs::exists(executablePath) &&
        fs::is_regular_file(executablePath);

    if (!result.available)
    {
        return result;
    }

    result.binarySizeBytes =
        static_cast<long>(fs::file_size(executablePath));

    string command =
        "/usr/bin/time -v \"" +
        executablePath.string() +
        "\" 2>&1";

    auto startTime = chrono::high_resolution_clock::now();

    FILE* pipe = popen(command.c_str(), "r");

    if (!pipe)
    {
        return result;
    }

    char buffer[1024];
    string output;

    while (fgets(buffer, sizeof(buffer), pipe))
    {
        output += buffer;
    }

    pclose(pipe);

    auto endTime = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsedTime =
        endTime - startTime;

    result.elapsedSeconds = elapsedTime.count();

    istringstream stream(output);
    string line;

    while (getline(stream, line))
    {
        if (line.find("User time (seconds):") != string::npos)
        {
            result.userCpuSeconds =
                stod(line.substr(line.find(":") + 1));
        }
        else if (line.find("System time (seconds):") != string::npos)
        {
            result.systemCpuSeconds =
                stod(line.substr(line.find(":") + 1));
        }
        else if (line.find("Maximum resident set size (kbytes):") != string::npos)
        {
            result.peakMemoryKb =
                stol(line.substr(line.find(":") + 1));
        }
        else if (line.find("Exit status:") != string::npos)
        {
            result.exitStatus =
                stoi(line.substr(line.find(":") + 1));
        }
    }

    return result;
}
