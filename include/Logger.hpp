#pragma once // Chỉ đọc file này đúng 1 lần

#include <string>

enum class LogLevel
{
    INFO,
    WARN,
    ERROR
};

class Logger
{
public:
    void log(LogLevel level, const std::string &message);

private:
    std::string levelToString(LogLevel level);
};