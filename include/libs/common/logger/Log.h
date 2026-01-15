#pragma once

#include "libs/common/utils/LogMacros.h"
#include "libs/common/service/Accessor.h"

/**
 * @brief Convenience logging macros for PlantNanny firmware
 * 
 * These macros fetch the Logger from the service registry and use
 * the existing LOG_IF_AVAILABLE macro. If no logger is registered,
 * the message is silently dropped.
 * 
 * Usage:
 *   LOG_INFO("[MODULE] Message");
 *   LOG_ERROR("[MODULE] Error message");
 */

#define LOG_INFO(msg) \
    LOG_IF_AVAILABLE(common::service::get<common::logger::Logger>(), info, msg)

#define LOG_ERROR(msg) \
    LOG_IF_AVAILABLE(common::service::get<common::logger::Logger>(), error, msg)

#define LOG_WARN(msg) \
    LOG_IF_AVAILABLE(common::service::get<common::logger::Logger>(), warning, msg)

#define LOG_DEBUG(msg) \
    LOG_IF_AVAILABLE(common::service::get<common::logger::Logger>(), debug, msg)

#define LOG_CRITICAL(msg) \
    LOG_IF_AVAILABLE(common::service::get<common::logger::Logger>(), critical, msg)
