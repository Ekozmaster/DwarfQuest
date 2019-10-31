#include <iostream>
#include "Logger.h"

void Logger::SetLoggingLevel(unsigned int loggingLevel) {
    m_loggingLevel = loggingLevel;
}

void Logger::Error(std::string message) {
    if (m_loggingLevel >= LoggingLevelError) {
        std::cout << "[ERROR]: " << message << std::endl;
    }
}

void Logger::Warn(std::string message) {
    if (m_loggingLevel >= LoggingLevelWarn) {
        std::cout << "[WARNING]: " << message << std::endl;
    }
}
void Logger::Info(std::string message) {
    if (m_loggingLevel >= LoggingLevelInfo) {
        std::cout << "[INFO]: " << message << std::endl;
    }
}

unsigned int Logger::m_loggingLevel = Logger::LoggingLevelInfo;