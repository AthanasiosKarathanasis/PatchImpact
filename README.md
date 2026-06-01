# PatchImpact

PatchImpact is a C++17 command-line framework developed for the MSc thesis:

Measuring Software Patch Impact in Open Source Projects: A Heuristic-Based Evaluation Framework

The framework compares original and patched C/C++ artefacts using static, quality, and executable-level measurements. It produces heuristic patch impact scores, grades, risk levels, text reports, and CSV outputs.

## Main Features

- Compare original and patched C/C++ folders
- Analyse individual source files, executables, or folders
- Collect structural metrics such as complexity, functions, and source lines of code
- Use Cppcheck findings as static quality indicators
- Compare executables using runtime-related measurements
- Generate human-readable reports
- Export structured CSV results
- Support mass assessment of compatible artefacts

## Build

Run:

    cmake -S . -B build
    cmake --build build

## Example Usage

    ./patchimpact_v41 <original_folder> <patched_folder>
    ./patchimpact_v41 -rep report.txt <original_folder> <patched_folder>
    ./patchimpact_v41 -csv results.csv -ma <folder>
    ./patchimpact_v41 -dm <original_executable> <patched_executable>

## Thesis Context

This repository supports reproducibility for the PatchImpact framework. The thesis appendices include selected code excerpts and representative outputs, while this repository contains the framework source code and usage examples.

## Notes

PatchImpact is a research prototype. It supports patch-impact evaluation and decision support, but it does not prove semantic correctness and does not replace testing or code review.
