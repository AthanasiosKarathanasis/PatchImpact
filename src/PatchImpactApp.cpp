#include "PatchImpactApp.h"
#include "SourceFileCollector.h"
#include "CppcheckAnalyzer.h"
#include "ComplexityAnalyzer.h"
#include "LineAnalyzer.h"
#include "DiffAnalyzer.h"
#include "LevenshteinAnalyzer.h"
#include "MaintainabilityAnalyzer.h"
#include "BuildStatusAnalyzer.h"
#include "DynamicMeasurementsAnalyzer.h"
#include "BehavioralImpactCalculator.h"
#include "StructuralImpactCalculator.h"
#include "QualityImpactCalculator.h"
#include "OverallImpactCalculator.h"
#include "IndividualAssessmentAnalyzer.h"
#include "MassAssessmentAnalyzer.h"
#include "CsvExportManager.h"
#include "ReportCapture.h"
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstdio>
#include <array>
#include <sstream>
#include <cctype>

using namespace std;
namespace fs = std::filesystem;

struct MetricTotals
{
    int changedFiles = 0;
    int unchangedFiles = 0;
    int totalAddedLines = 0;
    int totalRemovedLines = 0;
    int totalOriginalComplexity = 0;
    int totalPatchedComplexity = 0;
    int totalOriginalFunctions = 0;
    int totalPatchedFunctions = 0;
    int totalOriginalCodeLines = 0;
    int totalPatchedCodeLines = 0;
    int totalOriginalCommentLines = 0;
    int totalPatchedCommentLines = 0;
    int totalOriginalBlankLines = 0;
    int totalPatchedBlankLines = 0;
    CppcheckResults totalOriginalCppcheck;
    CppcheckResults totalPatchedCppcheck;
    double totalSimilarityPercent = 0.0;
    int similarityMeasurements = 0;
    double totalMaintainabilityChange = 0.0;
    int maintainabilityMeasurements = 0;
};


bool validateFolder(const fs::path& folder, const string& label);



bool containsFile(const vector<fs::path>& files,
                  const fs::path& target);



int totalCppcheckFindings(const CppcheckResults& results);

void addCppcheckResults(CppcheckResults& total,
                        const CppcheckResults& current);

void printCppcheckResults(const string& label,
                          const CppcheckResults& results);

string normalizeFileName(const fs::path& file);
bool areSimilarFileNames(const fs::path& originalFile,
                         const fs::path& patchedFile);

void analyzeFilePair(const fs::path& originalFolder,
                     const fs::path& patchedFolder,
                     const fs::path& originalRelativeFile,
                     const fs::path& patchedRelativeFile,
                     const string& pairLabel,
                     MetricTotals& totals);

int analyzePossibleRenames(const fs::path& originalFolder,
                           const fs::path& patchedFolder,
                           const vector<fs::path>& originalFiles,
                           const vector<fs::path>& patchedFiles,
                           MetricTotals& totals);

void scanFolder(const fs::path& folder);

void compareFolders(const fs::path& originalFolder,
                    const fs::path& patchedFolder);



bool validateFolder(const fs::path& folder, const string& label)
{
    if (!fs::exists(folder))
    {
        cerr << "Error: " << label << " folder does not exist: " << folder << endl;
        return false;
    }

    if (!fs::is_directory(folder))
    {
        cerr << "Error: " << label << " path is not a directory: " << folder << endl;
        return false;
    }

    return true;
}

bool containsFile(const vector<fs::path>& files, const fs::path& target)
{
    return find(files.begin(), files.end(), target) != files.end();
}

int totalCppcheckFindings(const CppcheckResults& results)
{
    return results.errors
         + results.warnings
         + results.style
         + results.information;
}

void addCppcheckResults(CppcheckResults& total,
                        const CppcheckResults& current)
{
    total.errors      += current.errors;
    total.warnings    += current.warnings;
    total.style       += current.style;
    total.information += current.information;
}

void printCppcheckResults(const string& label,
                          const CppcheckResults& results)
{
    cout << "    " << label << " cppcheck errors:       " << results.errors << endl;
    cout << "    " << label << " cppcheck warnings:     " << results.warnings << endl;
    cout << "    " << label << " cppcheck style:        " << results.style << endl;
    cout << "    " << label << " cppcheck information:  " << results.information << endl;
    cout << "    " << label << " cppcheck total:        "
         << totalCppcheckFindings(results) << endl;
}

string normalizeFileName(const fs::path& file)
{
    string stem = file.stem().string();
    string normalized;

    for (char c : stem)
    {
        unsigned char uc = static_cast<unsigned char>(c);

        if (isalpha(uc))
        {
            normalized += static_cast<char>(tolower(uc));
        }
    }

    return normalized;
}

bool areSimilarFileNames(const fs::path& originalFile,
                         const fs::path& patchedFile)
{
    if (originalFile.extension() != patchedFile.extension())
    {
        return false;
    }

    string originalName = normalizeFileName(originalFile);
    string patchedName  = normalizeFileName(patchedFile);

    if (originalName.empty() || patchedName.empty())
    {
        return false;
    }

    return originalName == patchedName;
}

void analyzeFilePair(const fs::path& originalFolder,
                     const fs::path& patchedFolder,
                     const fs::path& originalRelativeFile,
                     const fs::path& patchedRelativeFile,
                     const string& pairLabel,
                     MetricTotals& totals)
{
    fs::path originalFile = originalFolder / originalRelativeFile;
    fs::path patchedFile  = patchedFolder / patchedRelativeFile;

    DiffAnalyzer diffAnalyzer;
    bool different =
        diffAnalyzer.filesAreDifferent(originalFile, patchedFile);

    ComplexityAnalyzer complexityAnalyzer;

    int originalComplexity =
        complexityAnalyzer.calculateComplexity(originalFile);

    int patchedComplexity =
        complexityAnalyzer.calculateComplexity(patchedFile);

    int complexityChange = patchedComplexity - originalComplexity;

    int originalFunctionCount =
        complexityAnalyzer.countFunctions(originalFile);

    int patchedFunctionCount =
        complexityAnalyzer.countFunctions(patchedFile);

    int functionChange = patchedFunctionCount - originalFunctionCount;

    LineAnalyzer lineAnalyzer;
    LevenshteinAnalyzer levenshteinAnalyzer;
    MaintainabilityAnalyzer maintainabilityAnalyzer;

    LineMetrics originalLineMetrics = lineAnalyzer.analyze(originalFile);
    LineMetrics patchedLineMetrics  = lineAnalyzer.analyze(patchedFile);
    LevenshteinResults levenshteinResults = levenshteinAnalyzer.analyze(originalFile, patchedFile);
    totals.totalSimilarityPercent += levenshteinResults.similarityPercent;
    totals.similarityMeasurements++;
    int codeLineChange = patchedLineMetrics.codeLines
                       - originalLineMetrics.codeLines;
    int commentLineChange = patchedLineMetrics.commentLines
                          - originalLineMetrics.commentLines;
    int blankLineChange = patchedLineMetrics.blankLines
                        - originalLineMetrics.blankLines;

    CppcheckAnalyzer cppcheckAnalyzer;

    CppcheckResults originalCppcheck = cppcheckAnalyzer.analyze(originalFile);

    CppcheckResults patchedCppcheck = cppcheckAnalyzer.analyze(patchedFile);

    double originalMaintainability = maintainabilityAnalyzer.calculate(
            originalComplexity,
            originalLineMetrics.codeLines,
            originalLineMetrics.commentLines,
            totalCppcheckFindings(originalCppcheck));

    double patchedMaintainability = maintainabilityAnalyzer.calculate(
            patchedComplexity,
            patchedLineMetrics.codeLines,
            patchedLineMetrics.commentLines,
            totalCppcheckFindings(patchedCppcheck));

    double maintainabilityChange = patchedMaintainability - originalMaintainability;
    totals.totalMaintainabilityChange +=
        maintainabilityChange;

    totals.maintainabilityMeasurements++;

    totals.totalOriginalComplexity += originalComplexity;
    totals.totalPatchedComplexity  += patchedComplexity;
    totals.totalOriginalFunctions  += originalFunctionCount;
    totals.totalPatchedFunctions   += patchedFunctionCount;
    totals.totalOriginalCodeLines += originalLineMetrics.codeLines;
    totals.totalPatchedCodeLines  += patchedLineMetrics.codeLines;
    totals.totalOriginalCommentLines += originalLineMetrics.commentLines;
    totals.totalPatchedCommentLines  += patchedLineMetrics.commentLines;
    totals.totalOriginalBlankLines += originalLineMetrics.blankLines;
    totals.totalPatchedBlankLines  += patchedLineMetrics.blankLines;

    addCppcheckResults(totals.totalOriginalCppcheck, originalCppcheck);
    addCppcheckResults(totals.totalPatchedCppcheck, patchedCppcheck);

    cout << "  " << pairLabel << ": "
         << originalRelativeFile
         << " -> "
         << patchedRelativeFile
         << endl;

    cout << "    Status: " << (different ? "changed" : "unchanged") << endl;
    cout << "    Original size: " << fs::file_size(originalFile) << " bytes" << endl;
    cout << "    Patched size:  " << fs::file_size(patchedFile) << " bytes" << endl;

    if (different)
    {
    	DiffSummary diff =
    	    diffAnalyzer.analyzeDiff(originalFile, patchedFile);

    	cout << "    Diff added lines:   "
    	     << diff.addedLines << endl;

    	cout << "    Diff removed lines: "
    	     << diff.removedLines << endl;

    	totals.totalAddedLines += diff.addedLines;
    	totals.totalRemovedLines += diff.removedLines;
    	totals.changedFiles++;
    }
    else
    {
        totals.unchangedFiles++;
    }

    cout << "    Original complexity: " << originalComplexity << endl;
    cout << "    Patched complexity:  " << patchedComplexity << endl;
    cout << "    Complexity change:   " << complexityChange << endl;

    cout << "    Original functions:  " << originalFunctionCount << endl;
    cout << "    Patched functions:   " << patchedFunctionCount << endl;
    cout << "    Function change:     " << functionChange << endl;

    cout << "    Original SLOC:       " << originalLineMetrics.codeLines << endl;
    cout << "    Patched SLOC:        " << patchedLineMetrics.codeLines << endl;
    cout << "    SLOC change:         " << codeLineChange << endl;

    cout << "    Original comments:   " << originalLineMetrics.commentLines << endl;
    cout << "    Patched comments:    " << patchedLineMetrics.commentLines << endl;
    cout << "    Comment change:      " << commentLineChange << endl;

    cout << "    Original blanks:     " << originalLineMetrics.blankLines << endl;
    cout << "    Patched blanks:      " << patchedLineMetrics.blankLines << endl;
    cout << "    Blank line change:   " << blankLineChange << endl;
    cout << "    Levenshtein distance: " << levenshteinResults.distance << endl;
    cout << "    Similarity:           " << levenshteinResults.similarityPercent << "%" << endl;
    cout << "    Original maintainability: " << originalMaintainability << endl;
    cout << "    Patched maintainability:  " << patchedMaintainability << endl;
    cout << "    Maintainability change:   " << maintainabilityChange << endl;
    printCppcheckResults("Original", originalCppcheck);
    printCppcheckResults("Patched", patchedCppcheck);

    cout << "    Cppcheck total change: "
         << totalCppcheckFindings(patchedCppcheck)
            - totalCppcheckFindings(originalCppcheck)
         << endl;
}

int analyzePossibleRenames(const fs::path& originalFolder,
                           const fs::path& patchedFolder,
                           const vector<fs::path>& originalFiles,
                           const vector<fs::path>& patchedFiles,
                           MetricTotals& totals)
{
    int possibleRenameCount = 0;

    cout << endl;
    cout << "Possible renamed or similarly named files with metrics:" << endl;

    for (const auto& originalFile : originalFiles)
    {
        if (containsFile(patchedFiles, originalFile))
        {
            continue;
        }

        for (const auto& patchedFile : patchedFiles)
        {
            if (containsFile(originalFiles, patchedFile))
            {
                continue;
            }

            if (areSimilarFileNames(originalFile, patchedFile))
            {
                analyzeFilePair(originalFolder,
                                patchedFolder,
                                originalFile,
                                patchedFile,
                                "POSSIBLE RENAME",
                                totals);

                possibleRenameCount++;
            }
        }
    }

    if (possibleRenameCount == 0)
    {
        cout << "  None detected" << endl;
    }

    return possibleRenameCount;
}

void scanFolder(const fs::path& folder)
{
	SourceFileCollector collector;
	vector<fs::path> files = collector.collect(folder);

    cout << "Scanning source files in: " << folder << endl;

    ComplexityAnalyzer complexityAnalyzer;
    CppcheckAnalyzer cppcheckAnalyzer;
    LineAnalyzer lineAnalyzer;

    for (const auto& file : files)
    {
        fs::path fullFile = folder / file;

        int complexity =
            complexityAnalyzer.calculateComplexity(fullFile);

        int functions =
            complexityAnalyzer.countFunctions(fullFile);

        LineMetrics lineMetrics = lineAnalyzer.analyze(fullFile);
        CppcheckResults cppcheck = cppcheckAnalyzer.analyze(fullFile);

        cout << "  " << file << endl;
        cout << "    pmccabe complexity: " << complexity << endl;
        cout << "    function count:     " << functions << endl;
        cout << "    SLOC:               " << lineMetrics.codeLines << endl;
        cout << "    comment lines:      " << lineMetrics.commentLines << endl;
        cout << "    blank lines:        " << lineMetrics.blankLines << endl;
        printCppcheckResults("File", cppcheck);
    }

    cout << "Total C/C++ source/header files found: "
         << files.size() << endl;
}

void compareFolders(const fs::path& originalFolder,
                    const fs::path& patchedFolder)
{
	SourceFileCollector collector;
	vector<fs::path> originalFiles = collector.collect(originalFolder);
	vector<fs::path> patchedFiles  = collector.collect(patchedFolder);

    int matchedFiles = 0;
    int onlyOriginal = 0;
    int onlyPatched  = 0;
    int possibleRenameCount = 0;

    MetricTotals matchedTotals;
    MetricTotals renameTotals;

    cout << "Comparing source files by relative path" << endl << endl;
    cout << "Matched files:" << endl;

    for (const auto& file : originalFiles)
    {
        if (containsFile(patchedFiles, file))
        {
            analyzeFilePair(originalFolder,
                            patchedFolder,
                            file,
                            file,
                            "MATCH",
                            matchedTotals);

            matchedFiles++;
        }
    }

    cout << endl << "Only in original:" << endl;

    for (const auto& file : originalFiles)
    {
        if (!containsFile(patchedFiles, file))
        {
            cout << "  ORIGINAL ONLY: " << file << endl;
            onlyOriginal++;
        }
    }

    cout << endl << "Only in patched:" << endl;

    for (const auto& file : patchedFiles)
    {
        if (!containsFile(originalFiles, file))
        {
            cout << "  PATCHED ONLY: " << file << endl;
            onlyPatched++;
        }
    }

    possibleRenameCount = analyzePossibleRenames(originalFolder,
                                                 patchedFolder,
                                                 originalFiles,
                                                 patchedFiles,
                                                 renameTotals);

    cout << endl;
    cout << "Comparison summary:" << endl;
    cout << "  Original files:                    " << originalFiles.size() << endl;
    cout << "  Patched files:                     " << patchedFiles.size() << endl;
    cout << "  Matched files:                     " << matchedFiles << endl;
    cout << "  Only original:                     " << onlyOriginal << endl;
    cout << "  Only patched:                      " << onlyPatched << endl;
    cout << "  Possible rename candidates:        " << possibleRenameCount << endl;

    cout << "  Matched changed files:             " << matchedTotals.changedFiles << endl;
    cout << "  Matched unchanged files:           " << matchedTotals.unchangedFiles << endl;
    cout << "  Matched diff added lines:          " << matchedTotals.totalAddedLines << endl;
    cout << "  Matched diff removed lines:        " << matchedTotals.totalRemovedLines << endl;
    cout << "  Matched original complexity:       " << matchedTotals.totalOriginalComplexity << endl;
    cout << "  Matched patched complexity:        " << matchedTotals.totalPatchedComplexity << endl;
    cout << "  Matched complexity change:         "
         << (matchedTotals.totalPatchedComplexity
             - matchedTotals.totalOriginalComplexity)
         << endl;
    cout << "  Matched original functions:        " << matchedTotals.totalOriginalFunctions << endl;
    cout << "  Matched patched functions:         " << matchedTotals.totalPatchedFunctions << endl;
    cout << "  Matched function change:           "
         << (matchedTotals.totalPatchedFunctions
             - matchedTotals.totalOriginalFunctions)
         << endl;
    cout << "  Matched original SLOC:             " << matchedTotals.totalOriginalCodeLines << endl;
    cout << "  Matched patched SLOC:              " << matchedTotals.totalPatchedCodeLines << endl;
    cout << "  Matched SLOC change:               "
         << (matchedTotals.totalPatchedCodeLines
             - matchedTotals.totalOriginalCodeLines)
         << endl;
    cout << "  Matched original comments:         " << matchedTotals.totalOriginalCommentLines << endl;
    cout << "  Matched patched comments:          " << matchedTotals.totalPatchedCommentLines << endl;
    cout << "  Matched comment change:            "
         << (matchedTotals.totalPatchedCommentLines
             - matchedTotals.totalOriginalCommentLines)
         << endl;
    cout << "  Matched original blanks:           " << matchedTotals.totalOriginalBlankLines << endl;
    cout << "  Matched patched blanks:            " << matchedTotals.totalPatchedBlankLines << endl;
    cout << "  Matched blank line change:         "
         << (matchedTotals.totalPatchedBlankLines
             - matchedTotals.totalOriginalBlankLines)
         << endl;
    cout << "  Matched original cppcheck total:   "
         << totalCppcheckFindings(matchedTotals.totalOriginalCppcheck)
         << endl;
    cout << "  Matched patched cppcheck total:    "
         << totalCppcheckFindings(matchedTotals.totalPatchedCppcheck)
         << endl;
    cout << "  Matched cppcheck total change:     "
         << totalCppcheckFindings(matchedTotals.totalPatchedCppcheck)
            - totalCppcheckFindings(matchedTotals.totalOriginalCppcheck)
         << endl;

    cout << "  Rename changed files:              " << renameTotals.changedFiles << endl;
    cout << "  Rename unchanged files:            " << renameTotals.unchangedFiles << endl;
    cout << "  Rename diff added lines:           " << renameTotals.totalAddedLines << endl;
    cout << "  Rename diff removed lines:         " << renameTotals.totalRemovedLines << endl;
    cout << "  Rename original complexity:        " << renameTotals.totalOriginalComplexity << endl;
    cout << "  Rename patched complexity:         " << renameTotals.totalPatchedComplexity << endl;
    cout << "  Rename complexity change:          "
         << (renameTotals.totalPatchedComplexity
             - renameTotals.totalOriginalComplexity)
         << endl;
    cout << "  Rename original functions:         " << renameTotals.totalOriginalFunctions << endl;
    cout << "  Rename patched functions:          " << renameTotals.totalPatchedFunctions << endl;
    cout << "  Rename function change:            "
         << (renameTotals.totalPatchedFunctions
             - renameTotals.totalOriginalFunctions)
         << endl;
    cout << "  Rename original SLOC:              " << renameTotals.totalOriginalCodeLines << endl;
    cout << "  Rename patched SLOC:               " << renameTotals.totalPatchedCodeLines << endl;
    cout << "  Rename SLOC change:                "
         << (renameTotals.totalPatchedCodeLines
             - renameTotals.totalOriginalCodeLines)
         << endl;
    cout << "  Rename original comments:          " << renameTotals.totalOriginalCommentLines << endl;
    cout << "  Rename patched comments:           " << renameTotals.totalPatchedCommentLines << endl;
    cout << "  Rename comment change:             "
         << (renameTotals.totalPatchedCommentLines
             - renameTotals.totalOriginalCommentLines)
         << endl;
    cout << "  Rename original blanks:            " << renameTotals.totalOriginalBlankLines << endl;
    cout << "  Rename patched blanks:             " << renameTotals.totalPatchedBlankLines << endl;
    cout << "  Rename blank line change:          "
         << (renameTotals.totalPatchedBlankLines
             - renameTotals.totalOriginalBlankLines)
         << endl;
    cout << "  Rename original cppcheck total:    "
         << totalCppcheckFindings(renameTotals.totalOriginalCppcheck)
         << endl;
    cout << "  Rename patched cppcheck total:     "
         << totalCppcheckFindings(renameTotals.totalPatchedCppcheck)
         << endl;
    cout << "  Rename cppcheck total change:      "
         << totalCppcheckFindings(renameTotals.totalPatchedCppcheck)
            - totalCppcheckFindings(renameTotals.totalOriginalCppcheck)
         << endl;
    double averageSimilarityPercent = 100.0;

    if (matchedTotals.similarityMeasurements > 0)
    {
        averageSimilarityPercent =
            matchedTotals.totalSimilarityPercent /
            matchedTotals.similarityMeasurements;
    }

    StructuralImpactCalculator structuralCalculator;

    StructuralImpact structuralImpact =
        structuralCalculator.calculate(
            matchedTotals.totalOriginalComplexity,
            matchedTotals.totalPatchedComplexity,
            matchedTotals.totalOriginalFunctions,
            matchedTotals.totalPatchedFunctions,
            matchedTotals.totalOriginalCodeLines,
            matchedTotals.totalPatchedCodeLines,
            averageSimilarityPercent);

    cout << endl;
    cout << "Structural impact:" << endl;

    cout << "  Complexity change percent:   "
         << structuralImpact.complexityChangePercent << "%" << endl;

    cout << "  Function change percent:     "
         << structuralImpact.functionChangePercent << "%" << endl;

    cout << "  SLOC change percent:         "
         << structuralImpact.slocChangePercent << "%" << endl;

    cout << "  Average similarity percent:  "
         << structuralImpact.levenshteinSimilarityPercent << "%" << endl;

    cout << "  Structural score:            "
         << structuralImpact.structuralScore << "/100" << endl;
    double averageMaintainabilityChange = 0.0;

    if (matchedTotals.maintainabilityMeasurements > 0)
    {
        averageMaintainabilityChange =
            matchedTotals.totalMaintainabilityChange /
            matchedTotals.maintainabilityMeasurements;
    }

    int cppcheckChange =
        totalCppcheckFindings(matchedTotals.totalPatchedCppcheck)
        - totalCppcheckFindings(matchedTotals.totalOriginalCppcheck);

    QualityImpactCalculator qualityCalculator;

    QualityImpact qualityImpact =
        qualityCalculator.calculate(
            averageMaintainabilityChange,
            cppcheckChange,
            true,
            true);

    cout << endl;
    cout << "Quality impact:" << endl;

    cout << "  Average maintainability change: "
         << qualityImpact.maintainabilityChange << endl;

    cout << "  Cppcheck findings change:       "
         << qualityImpact.cppcheckChange << endl;

    cout << "  Build status changed:           "
         << (qualityImpact.buildStatusChanged ? "yes" : "no") << endl;

    cout << "  Patched build failed:           "
         << (qualityImpact.patchedBuildFailed ? "yes" : "no") << endl;

    cout << "  Quality score:                  "
         << qualityImpact.qualityScore << "/100" << endl;


    OverallImpactCalculator overallCalculator;

    OverallImpact overallImpact =
        overallCalculator.calculate(
            structuralImpact.structuralScore,
            qualityImpact.qualityScore);

    cout << endl;
    cout << "Overall patch evaluation:" << endl;

    cout << "  Static/structural score:        "
         << overallImpact.structuralScore << "/100" << endl;

    /*
	cout << "  Dynamic/behavioral score:       "
         << overallImpact.behavioralScore << "/100" << endl;
	*/
    cout << "  Dynamic/behavioral score:       "
         << "N/A (not measured)" << endl;

    cout << "  To perform dynamic analysis:    "
         << "./patchimpact -dm <original_executable> <patched_executable>"
         << endl;


    cout << "  Quality score:                  "
         << overallImpact.qualityScore << "/100" << endl;

    cout << "  Overall patch score:            "
         << overallImpact.overallScore << "/100" << endl;

    cout << "  Grade:                          "
         << overallImpact.grade << endl;

    cout << "  Risk level:                     "
         << overallImpact.riskLevel << endl;
}

void runBuildAnalysis(const fs::path& originalFolder,
                      const fs::path& patchedFolder)
{
    BuildStatusAnalyzer buildAnalyzer;

    BuildStatus originalStatus =
        buildAnalyzer.analyze(originalFolder);

    BuildStatus patchedStatus =
        buildAnalyzer.analyze(patchedFolder);

    cout << endl;
    cout << "Build analysis:" << endl;

    cout << "  Original build success: "
         << (originalStatus.success ? "yes" : "no") << endl;

    cout << "  Original build exit code: "
         << originalStatus.exitCode << endl;

    cout << "  Patched build success:  "
         << (patchedStatus.success ? "yes" : "no") << endl;

    cout << "  Patched build exit code:  "
         << patchedStatus.exitCode << endl;
}

void runDynamicMeasurements(const fs::path& originalExecutable,
                            const fs::path& patchedExecutable)
{
    DynamicMeasurementsAnalyzer analyzer;

    DynamicMeasurements original =
        analyzer.analyze(originalExecutable);

    DynamicMeasurements patched =
        analyzer.analyze(patchedExecutable);
    BehavioralImpactCalculator impactCalculator;

    BehavioralImpact impact =
        impactCalculator.calculate(
            original,
            patched);

    cout << endl;
    cout << "Dynamic measurements:" << endl;

    cout << "  Original executable available: "
         << (original.available ? "yes" : "no") << endl;
    cout << "  Original elapsed seconds:      "
         << original.elapsedSeconds << endl;
    cout << "  Original user CPU seconds:     "
         << original.userCpuSeconds << endl;
    cout << "  Original system CPU seconds:   "
         << original.systemCpuSeconds << endl;
    cout << "  Original peak memory KB:       "
         << original.peakMemoryKb << endl;
    cout << "  Original binary size bytes:    "
         << original.binarySizeBytes << endl;
    cout << "  Original exit status:          "
         << original.exitStatus << endl;

    cout << "  Patched executable available:  "
         << (patched.available ? "yes" : "no") << endl;
    cout << "  Patched elapsed seconds:       "
         << patched.elapsedSeconds << endl;
    cout << "  Patched user CPU seconds:      "
         << patched.userCpuSeconds << endl;
    cout << "  Patched system CPU seconds:    "
         << patched.systemCpuSeconds << endl;
    cout << "  Patched peak memory KB:        "
         << patched.peakMemoryKb << endl;
    cout << "  Patched binary size bytes:     "
         << patched.binarySizeBytes << endl;
    cout << "  Patched exit status:           "
         << patched.exitStatus << endl;


    double elapsedChange =
        patched.elapsedSeconds - original.elapsedSeconds;

    double userCpuChange =
        patched.userCpuSeconds - original.userCpuSeconds;

    double systemCpuChange =
        patched.systemCpuSeconds - original.systemCpuSeconds;

    long memoryChange =
        patched.peakMemoryKb - original.peakMemoryKb;

    cout << endl;
    cout << "Dynamic comparison:" << endl;

    cout << "  Elapsed time change seconds: "
         << elapsedChange << endl;

    if (original.elapsedSeconds != 0.0)
    {
        cout << "  Elapsed time change percent: "
             << (elapsedChange / original.elapsedSeconds) * 100.0
             << "%" << endl;
    }

    cout << "  User CPU change seconds:     "
         << userCpuChange << endl;

    cout << "  System CPU change seconds:   "
         << systemCpuChange << endl;

    cout << "  Peak memory change KB:       "
         << memoryChange << endl;

    long binarySizeChangeBytes =
        patched.binarySizeBytes - original.binarySizeBytes;

    double binarySizeChangePercent = 0.0;

    if (original.binarySizeBytes > 0)
    {
        binarySizeChangePercent =
            ((double)binarySizeChangeBytes / original.binarySizeBytes) * 100.0;
    }

    cout << "  Binary size change bytes:    "
         << binarySizeChangeBytes << endl;

    cout << "  Binary size change percent:  "
         << binarySizeChangePercent << "%" << endl;

    cout << "  Exit status changed:         "
         << (original.exitStatus != patched.exitStatus ? "yes" : "no")
         << endl;


    cout << endl;
    cout << "Dynamic impact:" << endl;

    cout << "  Elapsed change percent:      "
         << impact.elapsedChangePercent
         << "%" << endl;

    cout << "  CPU change percent:          "
         << impact.cpuChangePercent
         << "%" << endl;

    cout << "  Memory change percent:       "
         << impact.memoryChangePercent
         << "%" << endl;
    cout << "  Binary size change percent:  "
         << impact.binarySizeChangePercent << "%" << endl;

    cout << "  Exit status changed:         "
         << (impact.exitStatusChanged ? "yes" : "no")
         << endl;

    cout << "  Dynamic impact score:        "
         << impact.behavioralScore
         << "/100" << endl;

    cout << "  Grade:                       "
         << impact.grade << endl;

    cout << "  Risk level:                  "
         << impact.riskLevel << endl;
}

int PatchImpactApp::run(int argc, char* argv[])
{
    if (argc == 2)
    {
        string mode = argv[1];
        if (mode == "-V" ||
                    mode == "--version" ||
                    mode == "-version" ||
                    mode == "-v")
        {
            cout << "PatchImpact v0.41.0" << endl;
            cout << endl;
            cout << "Measuring Software Patch Impact in Open Source Projects :" << endl;
            cout << "A Heuristic-Based Evaluation Framework" << endl;
            cout << "By Athanasios Karathanasis" << endl;
            cout << endl;
            return 0;
        }

        if (mode == "-h" ||
            mode == "--help" ||
            mode == "-help" ||
			mode == "-HELP" ||
			mode == "-Help" ||
			mode == "-H" )
        {
            cout << "PatchImpact v0.41.0" << endl;
            cout << endl;
            cout << "Measuring Software Patch Impact in Open Source Projects :" << endl;
            cout << "A Heuristic-Based Evaluation Framework" << endl;
            cout << "By Athanasios Karathanasis" << endl;
            cout << endl;
            cout << endl;
            cout << "Usage:" << endl;
            cout << "  ./patchimpact <source_folder>" << endl;
            cout << "  ./patchimpact <original_folder> <patched_folder>" << endl;
            cout << endl;

            cout << "Options:" << endl;

            cout << "  -ia, --individual-assessment" << endl;
            cout << "      Assess a single source file, executable, or folder." << endl;
            cout << endl;

            cout << "  -ma, --mass-assessment" << endl;
            cout << "      Assess all compatible artefacts in a folder." << endl;
            cout << endl;

            cout << "  -dm, --dynamic-measurements" << endl;
            cout << "      Perform dynamic executable analysis." << endl;
            cout << endl;

            cout << "  -ba, --build-analysis" << endl;
            cout << "      Perform build status analysis." << endl;
            cout << endl;

            cout << "  -rep, --report" << endl;
            cout << "      Save terminal output to a report file." << endl;
            cout << endl;
            cout << "  -csv, --csv" << endl;
            cout << "      Save structured CSV output to a file." << endl;
            cout << endl;

            cout << "Examples:" << endl;
            cout << "  ./patchimpact source_folder" << endl;
            cout << "  ./patchimpact original_folder patched_folder" << endl;
            cout << "  ./patchimpact -dm original_executable patched_executable" << endl;
            cout << "  ./patchimpact -ia source.cpp" << endl;
            cout << "  ./patchimpact -ia project_folder" << endl;
            cout << "  ./patchimpact -ma project_folder" << endl;
            cout << "  ./patchimpact -rep report.txt original_folder patched_folder" << endl;
            cout << "  ./patchimpact -rep report.txt -ia project_folder" << endl;
            cout << "  ./patchimpact -rep report.txt -ma project_folder" << endl;
            cout << "  ./patchimpact -rep report.txt -dm exe1 exe2" << endl;
            cout << "  ./patchimpact -csv results.csv -ia project_folder" << endl;
            cout << "  ./patchimpact -csv results.csv -ma project_folder" << endl;
            cout << "  ./patchimpact -csv results.csv -dm exe1 exe2" << endl;
            cout << endl;

            return 0;
        }
    }

    ReportCapture reportCapture;

    if (argc >= 4)
    {
        string firstArg = argv[1];

        if (firstArg == "-rep" || firstArg == "--report")
        {
            string reportPath = argv[2];

            reportCapture.start(reportPath);

            for (int i = 3; i < argc; i++)
            {
                argv[i - 2] = argv[i];
            }

            argc -= 2;
        }
    }
    if (argc >= 4)
    {
        string firstArg = argv[1];

        if (firstArg == "-csv" || firstArg == "--csv")
        {
            string csvPath = argv[2];

            CsvExportManager::instance().open(csvPath);

            for (int i = 3; i < argc; i++)
            {
                argv[i - 2] = argv[i];
            }

            argc -= 2;
        }
    }

    cout << "PatchImpact v0.41.0" << endl;
    cout << "Measuring Software Patch Impact in Open Source Projects :" << endl;
    cout << "A Heuristic-Based Evaluation Framework" << endl;
    cout << "By Athanasios Karathanasis" << endl;

    if (argc == 3)
    {
        string mode = argv[1];

        if (mode == "-ia" || mode == "--individual-assessment")
        {
            IndividualAssessmentAnalyzer analyzer;
            analyzer.analyze(argv[2]);
            return 0;
        }

        if (mode == "-ma" || mode == "--mass-assessment")
        {
            MassAssessmentAnalyzer analyzer;
            analyzer.analyze(argv[2]);
            return 0;
        }
    }

    if (argc == 2)
    {
        fs::path folder = argv[1];

        if (!validateFolder(folder, "Source"))
        {
            return 1;
        }

        cout << "Single folder analysis mode" << endl;
        scanFolder(folder);
    }
    else if (argc == 3 || argc == 4)
    {
        bool buildAnalysisMode = false;

        fs::path originalFolder;
        fs::path patchedFolder;

        if (argc == 4)
        {
            string mode = argv[1];

            if (mode == "-ba" || mode == "--build-analysis")
            {
                buildAnalysisMode = true;
                originalFolder = argv[2];
                patchedFolder  = argv[3];
            }
            else if (mode == "-dm" || mode == "--dynamic-measurements")
            {
                runDynamicMeasurements(argv[2], argv[3]);
                return 0;
            }
            else
            {
                cout << "Unknown option: " << mode << endl;
                return 1;
            }
        }
        else
        {
            originalFolder = argv[1];
            patchedFolder  = argv[2];
        }

        if (!validateFolder(originalFolder, "Original"))
        {
            return 1;
        }

        if (!validateFolder(patchedFolder, "Patched"))
        {
            return 1;
        }

        cout << "Folder comparison mode" << endl;
        cout << "Original folder: " << originalFolder << endl;
        cout << "Patched folder:  " << patchedFolder << endl << endl;

        compareFolders(originalFolder, patchedFolder);
        if (buildAnalysisMode)
        {
            runBuildAnalysis(originalFolder, patchedFolder);
        }
    }
    else
    {
    	cout << "Wrong argument number" << endl;
    	cout << "Use --help or -h for usage information." << endl;
        return 1;
    }

    return 0;
}
