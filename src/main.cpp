#include "Logger.hpp"

int main()
{
    Logger logger;
    logger.log(LogLevel::INFO, "Information log");
    logger.log(LogLevel::WARN, "Warning log");
    logger.log(LogLevel::ERROR, "Error log");

    return 0;
}