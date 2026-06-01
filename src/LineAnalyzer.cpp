#include "LineAnalyzer.h"

#include <fstream>
#include <algorithm>
#include <cctype>

using namespace std;

string LineAnalyzer::trim(const string& text)
{
    auto begin = find_if_not(text.begin(), text.end(),
                             [](unsigned char c)
                             {
                                 return isspace(c);
                             });

    auto end = find_if_not(text.rbegin(), text.rend(),
                           [](unsigned char c)
                           {
                               return isspace(c);
                           }).base();

    if (begin >= end)
    {
        return "";
    }

    return string(begin, end);
}

LineMetrics LineAnalyzer::analyze(const fs::path& file)
{
    LineMetrics metrics;

    ifstream input(file);

    if (!input)
    {
        return metrics;
    }

    string line;
    bool insideBlockComment = false;

    while (getline(input, line))
    {
        string trimmed = trim(line);

        if (trimmed.empty())
        {
            metrics.blankLines++;
            continue;
        }

        if (insideBlockComment)
        {
            metrics.commentLines++;

            if (trimmed.find("*/") != string::npos)
            {
                insideBlockComment = false;
            }

            continue;
        }

        if (trimmed.rfind("//", 0) == 0)
        {
            metrics.commentLines++;
            continue;
        }

        if (trimmed.rfind("/*", 0) == 0)
        {
            metrics.commentLines++;

            if (trimmed.find("*/") == string::npos)
            {
                insideBlockComment = true;
            }

            continue;
        }

        metrics.codeLines++;
    }

    return metrics;
}
