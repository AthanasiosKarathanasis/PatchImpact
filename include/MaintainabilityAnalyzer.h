#pragma once

class MaintainabilityAnalyzer
{
public:
    double calculate(int complexity,
                     int sloc,
                     int comments,
                     int cppcheckFindings);
};
