#include "IndividualAssessmentAnalyzer.h"

#include "ComplexityAnalyzer.h"
#include "LineAnalyzer.h"
#include "CppcheckAnalyzer.h"
#include "MaintainabilityAnalyzer.h"
#include "DynamicMeasurementsAnalyzer.h"
#include "CsvExportManager.h"

#include <iostream>

using namespace std;

bool IndividualAssessmentAnalyzer::isSourceFile(const fs::path& filePath)
{
    string ext = filePath.extension().string();

    return ext == ".c" ||
           ext == ".cc" ||
           ext == ".cpp" ||
           ext == ".cxx" ||
           ext == ".h" ||
           ext == ".hpp" ||
           ext == ".hh" ||
           ext == ".hxx";
}

bool IndividualAssessmentAnalyzer::isExecutableFile(const fs::path& filePath)
{
    if (!fs::is_regular_file(filePath))
    {
        return false;
    }

    fs::perms permissions = fs::status(filePath).permissions();

    return (permissions & fs::perms::owner_exec) != fs::perms::none ||
           (permissions & fs::perms::group_exec) != fs::perms::none ||
           (permissions & fs::perms::others_exec) != fs::perms::none;
}

void IndividualAssessmentAnalyzer::analyzeSourceFile(const fs::path& filePath)
{
    ComplexityAnalyzer complexityAnalyzer;
    LineAnalyzer lineAnalyzer;
    CppcheckAnalyzer cppcheckAnalyzer;
    MaintainabilityAnalyzer maintainabilityAnalyzer;

    int complexity =
        complexityAnalyzer.calculateComplexity(filePath);

    int functions =
        complexityAnalyzer.countFunctions(filePath);

    LineMetrics lineMetrics =
        lineAnalyzer.analyze(filePath);

    CppcheckResults cppcheck =
        cppcheckAnalyzer.analyze(filePath);

    int cppcheckTotal =
        cppcheck.errors +
        cppcheck.warnings +
        cppcheck.style +
        cppcheck.information;

    double maintainability =
        maintainabilityAnalyzer.calculate(
            complexity,
            lineMetrics.codeLines,
            lineMetrics.commentLines,
            cppcheckTotal);

    long fileSizeBytes =
        static_cast<long>(fs::file_size(filePath));

    cout << endl;
    cout << "Source file assessment:" << endl;
    cout << "  Path:                  " << filePath << endl;
    cout << "  File size bytes:       " << fileSizeBytes << endl;
    cout << "  Cyclomatic complexity: " << complexity << endl;
    cout << "  Function count:        " << functions << endl;
    cout << "  SLOC:                  " << lineMetrics.codeLines << endl;
    cout << "  Comment lines:         " << lineMetrics.commentLines << endl;
    cout << "  Blank lines:           " << lineMetrics.blankLines << endl;
    cout << "  Maintainability:       " << maintainability << endl;
    cout << "  Cppcheck errors:       " << cppcheck.errors << endl;
    cout << "  Cppcheck warnings:     " << cppcheck.warnings << endl;
    cout << "  Cppcheck style:        " << cppcheck.style << endl;
    cout << "  Cppcheck information:  " << cppcheck.information << endl;
    cout << "  Cppcheck total:        " << cppcheckTotal << endl;

    if (CsvExportManager::instance().isEnabled())
    {
        CsvExportManager::instance().writeIndividualSourceRow(
            filePath,
            fileSizeBytes,
            complexity,
            functions,
            lineMetrics.codeLines,
            lineMetrics.commentLines,
            lineMetrics.blankLines,
            maintainability,
            cppcheck.errors,
            cppcheck.warnings,
            cppcheck.style,
            cppcheck.information,
            cppcheckTotal);
    }
}

void IndividualAssessmentAnalyzer::analyzeExecutableFile(const fs::path& filePath)
{
    DynamicMeasurementsAnalyzer analyzer;

    DynamicMeasurements measurements =
        analyzer.analyze(filePath);

    cout << endl;
    cout << "Executable assessment:" << endl;
    cout << "  Path:                  " << filePath << endl;
    cout << "  Available:             "
         << (measurements.available ? "yes" : "no") << endl;
    cout << "  Binary size bytes:     "
         << measurements.binarySizeBytes << endl;
    cout << "  Elapsed seconds:       "
         << measurements.elapsedSeconds << endl;
    cout << "  User CPU seconds:      "
         << measurements.userCpuSeconds << endl;
    cout << "  System CPU seconds:    "
         << measurements.systemCpuSeconds << endl;
    cout << "  Peak memory KB:        "
         << measurements.peakMemoryKb << endl;
    cout << "  Exit status:           "
         << measurements.exitStatus << endl;

    if (CsvExportManager::instance().isEnabled())
    {
        CsvExportManager::instance().writeIndividualExecutableRow(
            filePath,
            measurements.binarySizeBytes,
            measurements.elapsedSeconds,
            measurements.userCpuSeconds,
            measurements.systemCpuSeconds,
            measurements.peakMemoryKb,
            measurements.exitStatus);
    }
}

void IndividualAssessmentAnalyzer::analyze(const fs::path& targetPath)
{
    if (!fs::exists(targetPath))
    {
        cerr << "Error: path does not exist: " << targetPath << endl;
        return;
    }

    cout << "Individual assessment mode" << endl;
    cout << "Target path: " << targetPath << endl;

    if (CsvExportManager::instance().isEnabled())
    {
        CsvExportManager::instance().writeIndividualHeader();
    }

    if (fs::is_regular_file(targetPath))
    {
        if (isSourceFile(targetPath))
        {
            analyzeSourceFile(targetPath);
        }
        else if (isExecutableFile(targetPath))
        {
            analyzeExecutableFile(targetPath);
        }
        else
        {
            cout << "No applicable metrics for file: "
                 << targetPath << endl;
        }

        return;
    }

    if (!fs::is_directory(targetPath))
    {
        cerr << "Error: path is not a file or directory: "
             << targetPath << endl;
        return;
    }

    for (const auto& entry : fs::recursive_directory_iterator(targetPath))
    {
        if (!fs::is_regular_file(entry.path()))
        {
            continue;
        }

        if (isSourceFile(entry.path()))
        {
            analyzeSourceFile(entry.path());
        }
        else if (isExecutableFile(entry.path()))
        {
            analyzeExecutableFile(entry.path());
        }
    }
}
