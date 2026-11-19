#include "Logger.hpp"
#include <iostream>

std::string Logger::levelToString(LogLevel level)
{
    switch (level)
    {
    case LogLevel::INFO:
        return "INFO";
    case LogLevel::ERROR:
        return "ERROR";
    case LogLevel::WARN:
        return "WARN";
    default:
        return "UNKNOWN";
    }
}

void Logger::log(LogLevel level, const std::string &message)
{
    std::cout << "[" << levelToString(level) << "]: " << message << "\n";
}