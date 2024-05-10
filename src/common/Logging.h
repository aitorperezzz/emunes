#ifndef COMMON_LOGGING_H
#define COMMON_LOGGING_H

#include <cstdint>
#include <string>

namespace common
{

/// @brief Different log levels
enum class LogLevel : int
{
    ERROR,
    WARNING,
    INFO,
    DEBUG,
};

/// @brief Log the provided message with the provided level
/// @param level The log level
/// @param message The message
void Log(const LogLevel level, const std::string &message);

/// @brief Return a string with the hex value prepended by 0x and padded the requested number of bytes
/// @param value
/// @param size
/// @return
std::string print_hex(const uint16_t value, const size_t size);

} // namespace common

#endif