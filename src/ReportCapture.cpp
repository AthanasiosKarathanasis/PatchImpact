#include "ReportCapture.h"

ReportCaptureBuffer::ReportCaptureBuffer(
    std::streambuf* terminalBuffer,
    const std::string& reportPath)
    : terminalBuffer(terminalBuffer),
      reportFile(reportPath)
{
}

int ReportCaptureBuffer::overflow(int character)
{
    if (character == EOF)
    {
        return !EOF;
    }

    if (terminalBuffer)
    {
        terminalBuffer->sputc(character);
    }

    if (reportFile.is_open())
    {
        reportFile.put(static_cast<char>(character));
    }

    return character;
}

int ReportCaptureBuffer::sync()
{
    if (terminalBuffer)
    {
        terminalBuffer->pubsync();
    }

    if (reportFile.is_open())
    {
        reportFile.flush();
    }

    return 0;
}

ReportCapture::~ReportCapture()
{
    stop();
}

bool ReportCapture::start(const std::string& reportPath)
{
    if (captureBuffer != nullptr)
    {
        return false;
    }

    originalCoutBuffer = std::cout.rdbuf();

    captureBuffer =
        new ReportCaptureBuffer(originalCoutBuffer, reportPath);

    std::cout.rdbuf(captureBuffer);

    return true;
}

void ReportCapture::stop()
{
    if (captureBuffer != nullptr)
    {
        std::cout.rdbuf(originalCoutBuffer);

        delete captureBuffer;
        captureBuffer = nullptr;
        originalCoutBuffer = nullptr;
    }
}
