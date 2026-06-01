cmake_minimum_required(VERSION 3.10)

project(patchimpact_v41
    VERSION 0.41.0
    DESCRIPTION "PatchImpact OOP Refactor"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/PatchImpactApp.cpp
    src/SourceFileCollector.cpp
    src/CppcheckAnalyzer.cpp
    src/ComplexityAnalyzer.cpp
    src/LineAnalyzer.cpp
    src/DiffAnalyzer.cpp
    src/LevenshteinAnalyzer.cpp
    src/MaintainabilityAnalyzer.cpp
    src/BuildStatusAnalyzer.cpp
    src/DynamicMeasurementsAnalyzer.cpp
    src/BehavioralImpactCalculator.cpp
    src/StructuralImpactCalculator.cpp
    src/QualityImpactCalculator.cpp
    src/OverallImpactCalculator.cpp
    src/IndividualAssessmentAnalyzer.cpp
    src/MassAssessmentAnalyzer.cpp
    src/ReportCapture.cpp
    src/CsvExportManager.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)