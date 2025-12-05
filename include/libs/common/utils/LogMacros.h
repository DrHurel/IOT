#pragma once

#include "libs/common/logger/Logger.h"
#include "libs/common/service/Accessor.h"

namespace common::utils
{
#define LOG_IF_AVAILABLE(logger, level, message) \
    if ((logger).is_available())                 \
    {                                            \
        (logger)->level(message);                \
    }

} // namespace common::utils
