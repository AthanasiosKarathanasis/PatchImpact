#pragma once

#include <string>

struct OverallImpact
{
    double structuralScore = 0.0;
    double behavioralScore = 0.0;
    double qualityScore = 0.0;

    double overallScore = 0.0;

    std::string grade;
    std::string riskLevel;
};

class OverallImpactCalculator
{
public:
   /* OverallImpact calculate(
        double structuralScore,
        double behavioralScore,
        double qualityScore);
    */
	OverallImpact calculate(
	    double structuralScore,
	    double qualityScore);
};
