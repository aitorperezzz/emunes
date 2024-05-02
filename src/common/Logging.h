#ifndef COMMON_LOGGING_H
#define COMMON_LOGGING_H

#include <string>

namespace common
{
    enum class LogLevel : int
    {
        ERROR,
        WARNING,
        INFO,
        DEBUG,
    };

    void Log(const LogLevel level, const std::string &message)
    {
    }
}

#endif