#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "Logging.h"

namespace common
{

/// Flag indicating if the console log should be muted
static bool muted = false;

void Log(const LogLevel level, const std::string &message)
{
    if (muted)
    {
        return;
    }

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
    result << std::setfill('0') << std::setw(size * 2) << std::hex << std::uppercase << +value;
    return result.str();
}

void mute()
{
    muted = true;
}

void unmute()
{
    muted = false;
}

void LogFile::set_filename(const std::string &filename)
{
    this->filename = filename;
}

void LogFile::dump()
{
    std::ofstream output_file(this->filename);
    if (!output_file.is_open())
    {
        Log(LogLevel::ERROR, "Could not open file to write: " + this->filename);
        return;
    }

    for (const auto &line : this->records)
    {
        output_file << line << std::endl;
    }

    output_file.close();
    Log(LogLevel::INFO, "Log written to file " + this->filename);
}

void LogFile::add_record(const std::string &record)
{
    this->records.push_back(record);
}

} // namespace common
