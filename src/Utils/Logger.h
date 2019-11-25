#pragma once
#include <string>

class Logger {
private:
    static unsigned int m_loggingLevel;
public:
    static const unsigned int LoggingLevelError = 0;
    static const unsigned int LoggingLevelWarn = 1;
    static const unsigned int LoggingLevelInfo = 2;
public:
    Logger() = delete;

    static void SetLoggingLevel(unsigned int loggingLevel);
    static void Error(std::string message);
    static void Warn(std::string message);
    static void Info(std::string message);
};