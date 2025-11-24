#pragma once

#include <libs/common/logger/Logger.h>
#include <PubSubClient.h>
namespace common::logger
{
    class MQTTLogger : public Logger
    {
    private:
        PubSubClient* _mqtt_client; // Non-owning pointer - client lifetime managed externally

    public:
        explicit MQTTLogger(PubSubClient* mqtt_client) : _mqtt_client(mqtt_client) {}

    protected:
        void log(LogLevel level, const char *message) override;
    };
} // namespace common::logger
