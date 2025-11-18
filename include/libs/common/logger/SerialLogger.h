#pragma once

#include <libs/common/logger/Logger.h>

namespace common::logger
{
    class SerialLogger :  public Logger
    {

    public:
        SerialLogger();
    protected:
        void log(LogLevel level, const char *message) override;
    };
}