#ifndef COMMON_LOGGING_H
#define COMMON_LOGGING_H

#include <cstdint>
#include <string>
#include <vector>

namespace common
{

/// @brief Different log levels
enum class LogLevel : uint8_t
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

/// @brief Return a string containing the provided hex value (in uppercase) and occupying a total
/// of size * 2 characters
std::string print_hex(const uint16_t value, const size_t size);

/// @brief Mute the console log
void mute();

/// @brief Unmute the console log
void unmute();

/// @brief Class representing the official NES log file
class LogFile
{
  public:
    /// @brief Decide the filename to output
    void set_filename(const std::string &filename);

    /// @brief Add a new record (a line) to the log file
    void add_record(const std::string &record);

    /// @brief Dump all the provided records to file
    void dump();

  private:
    /// @brief The output log filename that has been selected
    std::string filename;

    /// @brief The log records that have been added
    std::vector<std::string> records;
};

} // namespace common

#endif