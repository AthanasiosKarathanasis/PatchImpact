#include "MassAssessmentAnalyzer.h"

#include "ComplexityAnalyzer.h"
#include "LineAnalyzer.h"
#include "LevenshteinAnalyzer.h"
#include "MaintainabilityAnalyzer.h"
#include "CppcheckAnalyzer.h"
#include "StructuralImpactCalculator.h"
#include "QualityImpactCalculator.h"
#include "DynamicMeasurementsAnalyzer.h"
#include "BehavioralImpactCalculator.h"
#include "CsvExportManager.h"

#include <iostream>

using namespace std;

bool MassAssessmentAnalyzer::isSourceFile(const fs::path& filePath)
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

bool MassAssessmentAnalyzer::isExecutableFile(const fs::path& filePath)
{
    if (!fs::is_regular_file(filePath))
    {
        return false;
    }

    fs::perms permissions =
        fs::status(filePath).permissions();

    return (permissions & fs::perms::owner_exec) != fs::perms::none ||
           (permissions & fs::perms::group_exec) != fs::perms::none ||
           (permissions & fs::perms::others_exec) != fs::perms::none;
}

vector<fs::path> MassAssessmentAnalyzer::collectSourceFiles(
    const fs::path& folderPath)
{
    vector<fs::path> files;

    for (const auto& entry : fs::recursive_directory_iterator(folderPath))
    {
        if (!fs::is_regular_file(entry.path()))
        {
            continue;
        }

        if (isSourceFile(entry.path()))
        {
            files.push_back(entry.path());
        }
    }

    return files;
}

vector<fs::path> MassAssessmentAnalyzer::collectExecutableFiles(
    const fs::path& folderPath)
{
    vector<fs::path> files;

    for (const auto& entry : fs::recursive_directory_iterator(folderPath))
    {
        if (!fs::is_regular_file(entry.path()))
        {
            continue;
        }

        if (isExecutableFile(entry.path()) &&
            !isSourceFile(entry.path()))
        {
            files.push_back(entry.path());
        }
    }

    return files;
}

void MassAssessmentAnalyzer::assessSourcePair(
    const fs::path& originalFile,
    const fs::path& patchedFile)
{
    ComplexityAnalyzer complexityAnalyzer;
    LineAnalyzer lineAnalyzer;
    LevenshteinAnalyzer levenshteinAnalyzer;
    MaintainabilityAnalyzer maintainabilityAnalyzer;
    CppcheckAnalyzer cppcheckAnalyzer;

    int originalComplexity =
        complexityAnalyzer.calculateComplexity(originalFile);

    int patchedComplexity =
        complexityAnalyzer.calculateComplexity(patchedFile);

    int complexityChange =
        patchedComplexity - originalComplexity;

    int originalFunctions =
        complexityAnalyzer.countFunctions(originalFile);

    int patchedFunctions =
        complexityAnalyzer.countFunctions(patchedFile);

    int functionChange =
        patchedFunctions - originalFunctions;

    LineMetrics originalLines =
        lineAnalyzer.analyze(originalFile);

    LineMetrics patchedLines =
        lineAnalyzer.analyze(patchedFile);

    int slocChange =
        patchedLines.codeLines - originalLines.codeLines;

    LevenshteinResults similarity =
        levenshteinAnalyzer.analyze(originalFile, patchedFile);

    CppcheckResults originalCppcheck =
        cppcheckAnalyzer.analyze(originalFile);

    CppcheckResults patchedCppcheck =
        cppcheckAnalyzer.analyze(patchedFile);

    int originalCppcheckTotal =
        originalCppcheck.errors +
        originalCppcheck.warnings +
        originalCppcheck.style +
        originalCppcheck.information;

    int patchedCppcheckTotal =
        patchedCppcheck.errors +
        patchedCppcheck.warnings +
        patchedCppcheck.style +
        patchedCppcheck.information;

    int cppcheckChange =
        patchedCppcheckTotal - originalCppcheckTotal;

    double originalMaintainability =
        maintainabilityAnalyzer.calculate(
            originalComplexity,
            originalLines.codeLines,
            originalLines.commentLines,
            originalCppcheckTotal);

    double patchedMaintainability =
        maintainabilityAnalyzer.calculate(
            patchedComplexity,
            patchedLines.codeLines,
            patchedLines.commentLines,
            patchedCppcheckTotal);

    double maintainabilityChange =
        patchedMaintainability - originalMaintainability;

    StructuralImpactCalculator structuralCalculator;

    StructuralImpact structuralImpact =
        structuralCalculator.calculate(
            originalComplexity,
            patchedComplexity,
            originalFunctions,
            patchedFunctions,
            originalLines.codeLines,
            patchedLines.codeLines,
            similarity.similarityPercent);

    QualityImpactCalculator qualityCalculator;

    QualityImpact qualityImpact =
        qualityCalculator.calculate(
            maintainabilityChange,
            cppcheckChange,
            true,
            true);

    cout << endl;
    cout << "Mass source pair assessment:" << endl;
    cout << "  Assumed original:       " << originalFile << endl;
    cout << "  Assumed patched:        " << patchedFile << endl;

    cout << "  Original complexity:    " << originalComplexity << endl;
    cout << "  Patched complexity:     " << patchedComplexity << endl;
    cout << "  Complexity change:      "
         << complexityChange << endl;

    cout << "  Original functions:     " << originalFunctions << endl;
    cout << "  Patched functions:      " << patchedFunctions << endl;
    cout << "  Function change:        "
         << functionChange << endl;

    cout << "  Original SLOC:          " << originalLines.codeLines << endl;
    cout << "  Patched SLOC:           " << patchedLines.codeLines << endl;
    cout << "  SLOC change:            "
         << slocChange << endl;

    cout << "  Similarity percent:     "
         << similarity.similarityPercent << "%" << endl;

    cout << "  Maintainability change: "
         << maintainabilityChange << endl;

    cout << "  Cppcheck change:        "
         << cppcheckChange << endl;

    cout << "  Structural score:       "
         << structuralImpact.structuralScore << "/100" << endl;

    cout << "  Quality score:          "
         << qualityImpact.qualityScore << "/100" << endl;

    if (CsvExportManager::instance().isEnabled())
    {
        CsvExportManager::instance().writeMassSourcePairRow(
            originalFile,
            patchedFile,
            originalComplexity,
            patchedComplexity,
            complexityChange,
            originalFunctions,
            patchedFunctions,
            functionChange,
            originalLines.codeLines,
            patchedLines.codeLines,
            slocChange,
            similarity.similarityPercent,
            maintainabilityChange,
            cppcheckChange,
            structuralImpact.structuralScore,
            qualityImpact.qualityScore);
    }
}

void MassAssessmentAnalyzer::assessExecutablePair(
    const fs::path& originalExecutable,
    const fs::path& patchedExecutable)
{
    DynamicMeasurementsAnalyzer dynamicAnalyzer;

    DynamicMeasurements original =
        dynamicAnalyzer.analyze(originalExecutable);

    DynamicMeasurements patched =
        dynamicAnalyzer.analyze(patchedExecutable);

    BehavioralImpactCalculator impactCalculator;

    BehavioralImpact impact =
        impactCalculator.calculate(
            original,
            patched);

    double originalCpuTotal =
        original.userCpuSeconds + original.systemCpuSeconds;

    double patchedCpuTotal =
        patched.userCpuSeconds + patched.systemCpuSeconds;

    cout << endl;
    cout << "Mass executable pair assessment:" << endl;
    cout << "  Assumed original:       " << originalExecutable << endl;
    cout << "  Assumed patched:        " << patchedExecutable << endl;

    cout << "  Original elapsed:       "
         << original.elapsedSeconds << endl;

    cout << "  Patched elapsed:        "
         << patched.elapsedSeconds << endl;

    cout << "  Original CPU total:     "
         << originalCpuTotal << endl;

    cout << "  Patched CPU total:      "
         << patchedCpuTotal << endl;

    cout << "  Original memory KB:     "
         << original.peakMemoryKb << endl;

    cout << "  Patched memory KB:      "
         << patched.peakMemoryKb << endl;

    cout << "  Original binary bytes:  "
         << original.binarySizeBytes << endl;

    cout << "  Patched binary bytes:   "
         << patched.binarySizeBytes << endl;

    cout << "  Original exit status:   "
         << original.exitStatus << endl;

    cout << "  Patched exit status:    "
         << patched.exitStatus << endl;

    cout << "  Elapsed change percent: "
         << impact.elapsedChangePercent << "%" << endl;

    cout << "  CPU change percent:     "
         << impact.cpuChangePercent << "%" << endl;

    cout << "  Memory change percent:  "
         << impact.memoryChangePercent << "%" << endl;

    cout << "  Binary change percent:  "
         << impact.binarySizeChangePercent << "%" << endl;

    cout << "  Exit status changed:    "
         << (impact.exitStatusChanged ? "yes" : "no") << endl;

    cout << "  Dynamic score:          "
         << impact.behavioralScore << "/100" << endl;

    cout << "  Grade:                  "
         << impact.grade << endl;

    cout << "  Risk level:             "
         << impact.riskLevel << endl;

    if (CsvExportManager::instance().isEnabled())
    {
        CsvExportManager::instance().writeMassExecutablePairRow(
            originalExecutable,
            patchedExecutable,
            original.elapsedSeconds,
            patched.elapsedSeconds,
            originalCpuTotal,
            patchedCpuTotal,
            original.peakMemoryKb,
            patched.peakMemoryKb,
            original.binarySizeBytes,
            patched.binarySizeBytes,
            original.exitStatus,
            patched.exitStatus,
            impact.elapsedChangePercent,
            impact.cpuChangePercent,
            impact.memoryChangePercent,
            impact.binarySizeChangePercent,
            impact.exitStatusChanged,
            impact.behavioralScore,
            impact.grade,
            impact.riskLevel);
    }
}

void MassAssessmentAnalyzer::analyze(const fs::path& folderPath)
{
    if (!fs::exists(folderPath))
    {
        cerr << "Error: folder does not exist: "
             << folderPath << endl;
        return;
    }

    if (!fs::is_directory(folderPath))
    {
        cerr << "Error: mass assessment requires a folder: "
             << folderPath << endl;
        return;
    }

    vector<fs::path> sourceFiles =
        collectSourceFiles(folderPath);

    vector<fs::path> executableFiles =
        collectExecutableFiles(folderPath);

    cout << "Mass assessment mode" << endl;
    cout << "Target folder: " << folderPath << endl;
    cout << "Source files found: " << sourceFiles.size() << endl;
    cout << "Executable files found: " << executableFiles.size() << endl;

    if (CsvExportManager::instance().isEnabled())
    {
        CsvExportManager::instance().writeMassHeader();
    }

    for (size_t i = 0; i < sourceFiles.size(); i++)
    {
        for (size_t j = 0; j < sourceFiles.size(); j++)
        {
            if (i == j)
            {
                continue;
            }

            assessSourcePair(sourceFiles[i], sourceFiles[j]);
        }
    }

    for (size_t i = 0; i < executableFiles.size(); i++)
    {
        for (size_t j = 0; j < executableFiles.size(); j++)
        {
            if (i == j)
            {
                continue;
            }

            assessExecutablePair(executableFiles[i], executableFiles[j]);
        }
    }
}
