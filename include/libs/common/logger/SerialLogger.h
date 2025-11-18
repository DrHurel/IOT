#pragma once

#include <libs/common/logger/Logger.h>

namespace common::logger
{
    class SerialLogger : public Logger
    {
    protected:
        void log(LogLevel level, const char *message) override;
    };
}