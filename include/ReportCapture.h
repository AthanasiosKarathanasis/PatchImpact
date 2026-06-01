#pragma once

#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>

class ReportCaptureBuffer : public std::streambuf
{
private:
    std::streambuf* terminalBuffer;
    std::ofstream reportFile;

public:
    ReportCaptureBuffer(
        std::streambuf* terminalBuffer,
        const std::string& reportPath);

protected:
    int overflow(int character) override;
    int sync() override;
};

class ReportCapture
{
private:
    std::streambuf* originalCoutBuffer = nullptr;
    ReportCaptureBuffer* captureBuffer = nullptr;

public:
    ReportCapture() = default;
    ~ReportCapture();

    bool start(const std::string& reportPath);
    void stop();
};
