#pragma once // Run this file once time

#include <string>
#include <fstream>
#include <mutex>

enum class LogLevel
{
    INFO,
    WARN,
    ERROR
};

class Logger
{
public:
    static Logger &getInstance();

    Logger(const Logger&) = delete; 
    Logger &operator = (const Logger&) = delete; 

    void init(const std::string &filePath);
    void log(LogLevel level, const std::string &message);

private:
    Logger() = default;
    ~Logger();
    
    std::string levelToString(LogLevel level);
    std::ofstream logFile_; 
    std::mutex mutex_; 
};