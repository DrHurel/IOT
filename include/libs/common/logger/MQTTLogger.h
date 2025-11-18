#pragma once

#include <libs/common/logger/Logger.h>
#include <PubSubClient.h>
namespace common::logger
{
    class MQTTLogger : public Logger
    {

       PubSubClient* _mqtt_client;

    public:
        MQTTLogger(PubSubClient* mqtt_client) : _mqtt_client(mqtt_client) {}

    protected:
        void log(LogLevel level, const char *message) override;
    };
} // namespace common::logger
