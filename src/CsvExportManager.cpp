#include "CsvExportManager.h"

using namespace std;

CsvExportManager& CsvExportManager::instance()
{
    static CsvExportManager manager;
    return manager;
}

bool CsvExportManager::open(const fs::path& outputPath)
{
    file.open(outputPath);

    enabled = file.is_open();

    return enabled;
}

void CsvExportManager::close()
{
    if (file.is_open())
    {
        file.close();
    }

    enabled = false;
}

bool CsvExportManager::isEnabled() const
{
    return enabled;
}

string CsvExportManager::escape(const string& value)
{
    string escaped = "\"";

    for (char c : value)
    {
        if (c == '"')
        {
            escaped += "\"\"";
        }
        else
        {
            escaped += c;
        }
    }

    escaped += "\"";

    return escaped;
}

void CsvExportManager::writeRawLine(const string& line)
{
    if (!enabled || !file.is_open())
    {
        return;
    }

    file << line << endl;
}

void CsvExportManager::writeIndividualHeader()
{
    if (!enabled || !file.is_open())
    {
        return;
    }

    file << "path,type,file_size_bytes,complexity,function_count,sloc,"
         << "comment_lines,blank_lines,maintainability,"
         << "cppcheck_errors,cppcheck_warnings,cppcheck_style,"
         << "cppcheck_information,cppcheck_total,"
         << "binary_size_bytes,elapsed_seconds,user_cpu_seconds,"
         << "system_cpu_seconds,peak_memory_kb,exit_status"
         << endl;
}

void CsvExportManager::writeIndividualSourceRow(
    const fs::path& path,
    long fileSizeBytes,
    int complexity,
    int functionCount,
    int sloc,
    int commentLines,
    int blankLines,
    double maintainability,
    int cppcheckErrors,
    int cppcheckWarnings,
    int cppcheckStyle,
    int cppcheckInformation,
    int cppcheckTotal)
{
    if (!enabled || !file.is_open())
    {
        return;
    }

    file << escape(path.string()) << ","
         << "source" << ","
         << fileSizeBytes << ","
         << complexity << ","
         << functionCount << ","
         << sloc << ","
         << commentLines << ","
         << blankLines << ","
         << maintainability << ","
         << cppcheckErrors << ","
         << cppcheckWarnings << ","
         << cppcheckStyle << ","
         << cppcheckInformation << ","
         << cppcheckTotal << ","
         << ",,,,,"
         << endl;
}

void CsvExportManager::writeIndividualExecutableRow(
    const fs::path& path,
    long binarySizeBytes,
    double elapsedSeconds,
    double userCpuSeconds,
    double systemCpuSeconds,
    long peakMemoryKb,
    int exitStatus)
{
    if (!enabled || !file.is_open())
    {
        return;
    }

    file << escape(path.string()) << ","
         << "executable" << ","
         << ",,,,,,,,,,,,"
         << ","
         << binarySizeBytes << ","
         << elapsedSeconds << ","
         << userCpuSeconds << ","
         << systemCpuSeconds << ","
         << peakMemoryKb << ","
         << exitStatus
         << endl;
}



void CsvExportManager::writeMassHeader()
{
    if (!enabled || !file.is_open())
    {
        return;
    }

    file << "assessment_type,original_path,patched_path,"
         << "original_complexity,patched_complexity,complexity_change,"
         << "original_functions,patched_functions,function_change,"
         << "original_sloc,patched_sloc,sloc_change,"
         << "similarity_percent,maintainability_change,cppcheck_change,"
         << "structural_score,quality_score,"
         << "original_elapsed,patched_elapsed,"
         << "original_cpu_total,patched_cpu_total,"
         << "original_memory_kb,patched_memory_kb,"
         << "original_binary_bytes,patched_binary_bytes,"
         << "original_exit_status,patched_exit_status,"
         << "elapsed_change_percent,cpu_change_percent,"
         << "memory_change_percent,binary_change_percent,"
         << "exit_status_changed,dynamic_score,grade,risk_level"
         << endl;
}

void CsvExportManager::writeMassSourcePairRow(
    const fs::path& originalPath,
    const fs::path& patchedPath,
    int originalComplexity,
    int patchedComplexity,
    int complexityChange,
    int originalFunctions,
    int patchedFunctions,
    int functionChange,
    int originalSloc,
    int patchedSloc,
    int slocChange,
    double similarityPercent,
    double maintainabilityChange,
    int cppcheckChange,
    double structuralScore,
    double qualityScore)
{
    if (!enabled || !file.is_open())
    {
        return;
    }

    file << "source_pair" << ","
         << escape(originalPath.string()) << ","
         << escape(patchedPath.string()) << ","
         << originalComplexity << ","
         << patchedComplexity << ","
         << complexityChange << ","
         << originalFunctions << ","
         << patchedFunctions << ","
         << functionChange << ","
         << originalSloc << ","
         << patchedSloc << ","
         << slocChange << ","
         << similarityPercent << ","
         << maintainabilityChange << ","
         << cppcheckChange << ","
         << structuralScore << ","
         << qualityScore << ","
         << ",,,,,,,,,,,,,,,,,,"
         << endl;
}

void CsvExportManager::writeMassExecutablePairRow(
    const fs::path& originalPath,
    const fs::path& patchedPath,
    double originalElapsed,
    double patchedElapsed,
    double originalCpuTotal,
    double patchedCpuTotal,
    long originalMemoryKb,
    long patchedMemoryKb,
    long originalBinaryBytes,
    long patchedBinaryBytes,
    int originalExitStatus,
    int patchedExitStatus,
    double elapsedChangePercent,
    double cpuChangePercent,
    double memoryChangePercent,
    double binaryChangePercent,
    bool exitStatusChanged,
    double dynamicScore,
    const string& grade,
    const string& riskLevel)
{
    if (!enabled || !file.is_open())
    {
        return;
    }

    file << "executable_pair" << ","
         << escape(originalPath.string()) << ","
         << escape(patchedPath.string()) << ","
         << ",,,,,,,,,,,,,,"
         << ","
         << originalElapsed << ","
         << patchedElapsed << ","
         << originalCpuTotal << ","
         << patchedCpuTotal << ","
         << originalMemoryKb << ","
         << patchedMemoryKb << ","
         << originalBinaryBytes << ","
         << patchedBinaryBytes << ","
         << originalExitStatus << ","
         << patchedExitStatus << ","
         << elapsedChangePercent << ","
         << cpuChangePercent << ","
         << memoryChangePercent << ","
         << binaryChangePercent << ","
         << (exitStatusChanged ? "yes" : "no") << ","
         << dynamicScore << ","
         << escape(grade) << ","
         << escape(riskLevel)
         << endl;
}
