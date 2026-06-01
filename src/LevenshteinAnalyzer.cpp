#include "LevenshteinAnalyzer.h"

#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

string LevenshteinAnalyzer::readFile(const fs::path& file)
{
    ifstream input(file, ios::binary);

    if (!input)
    {
        return "";
    }

    string content;
    string line;

    while (getline(input, line))
    {
        content += line;
        content += '\n';
    }

    return content;
}

int LevenshteinAnalyzer::calculateDistance(const string& first,
                                           const string& second)
{
    const size_t firstSize = first.size();
    const size_t secondSize = second.size();

    vector<int> previous(secondSize + 1);
    vector<int> current(secondSize + 1);

    for (size_t j = 0; j <= secondSize; j++)
    {
        previous[j] = static_cast<int>(j);
    }

    for (size_t i = 1; i <= firstSize; i++)
    {
        current[0] = static_cast<int>(i);

        for (size_t j = 1; j <= secondSize; j++)
        {
            int cost = first[i - 1] == second[j - 1] ? 0 : 1;

            current[j] = min({
                previous[j] + 1,
                current[j - 1] + 1,
                previous[j - 1] + cost
            });
        }

        previous = current;
    }

    return previous[secondSize];
}

LevenshteinResults LevenshteinAnalyzer::analyze(
    const fs::path& originalFile,
    const fs::path& patchedFile)
{
    LevenshteinResults results;

    string originalContent = readFile(originalFile);
    string patchedContent = readFile(patchedFile);

    results.distance =
        calculateDistance(originalContent, patchedContent);

    size_t maxLength = max(originalContent.size(), patchedContent.size());

    if (maxLength == 0)
    {
        results.similarityPercent = 100.0;
    }
    else
    {
        results.similarityPercent =
            100.0 -
            (static_cast<double>(results.distance)
             / static_cast<double>(maxLength) * 100.0);
    }

    return results;
}
