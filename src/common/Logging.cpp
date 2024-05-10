#include <iomanip>
#include <iostream>
#include <sstream>

#include "Logging.h"

namespace common
{
void Log(const LogLevel level, const std::string &message)
{
    std::string level_string;
    switch (level)
    {
    case LogLevel::ERROR:
        level_string = "ERROR   ";
        break;
    case LogLevel::WARNING:
        level_string = "WARNING ";
        break;
    case LogLevel::INFO:
        level_string = "INFO    ";
        break;
    case LogLevel::DEBUG:
        level_string = "DEBUG   ";
        break;
    }
    std::cout << level_string << message << std::endl;
}

std::string print_hex(const uint16_t value, const size_t size)
{
    std::stringstream result;
    result << "0x" << std::setfill('0') << std::setw(size * 2) << std::hex << +value;
    return result.str();
}
} // namespace common
