#include "Logger.hpp"
#include <iostream>
#include<filesystem>

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
    std::lock_guard<std::mutex> lock(mutex_);
    std::string output = "[" + levelToString(level) + "]: " + message;
    std::cout << output << "\n";
    if (logFile_.is_open())
    {
        logFile_ << output << "\n";
    }
}

Logger &Logger::getInstance()
{
    static Logger instance;
    return instance;
}

Logger::~Logger()
{
    if (logFile_.is_open())
    {
        logFile_.close();
    }
}

void Logger::init(const std::string &filePath)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (logFile_.is_open())
    {
        return;
    }
    std::filesystem::path path(filePath);
    if(path.has_parent_path()){
        std::filesystem::create_directories(path.parent_path()); 
    }
    logFile_.open(filePath, std::ios::out | std::ios::app);

    if (!logFile_.is_open())
    {
        std::cerr << "Error When check file log!!\n";
    }
}