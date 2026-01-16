#pragma once

#include "libs/plant_nanny/services/mqtt/IMQTTService.h"
#include "libs/common/patterns/Result.h"
#include "libs/common/logger/Logger.h"
#include "libs/common/service/Accessor.h"
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <string>
#include <functional>

namespace plant_nanny::services::mqtt
{
    struct SensorReading
    {
        float temperatureC;
        float humidityPct;
        float luminosityPct;
    };

    enum class CommandType
    {
        Unknown,
        SendNow,
        PumpWater,
        SetInterval,
        Restart,
        OtaUpdate
    };

    struct Command
    {
        CommandType type;
        int durationMs;
        int amountMl;
        int intervalMs;
        std::string otaUrl;
    };

    class MQTTService : public IMQTTService
    {
    public:
        using ReadingCallback = std::function<SensorReading()>;
        using CommandCallback = std::function<void(const Command&)>;

    private:
        common::service::Accessor<common::logger::Logger> logger_;

        WiFiClient wifi_client_;
        PubSubClient mqtt_client_;

        std::string broker_host_;
        uint16_t broker_port_;
        std::string device_id_;
        std::string username_;
        std::string password_;

        bool initialized_;
        bool enabled_;
        uint32_t last_publish_time_;
        uint32_t last_reconnect_attempt_;
        uint32_t publish_interval_ms_;
        
        ReadingCallback reading_callback_;
        CommandCallback command_callback_;

        static constexpr uint32_t DEFAULT_PUBLISH_INTERVAL_MS = 60000;
        static constexpr uint32_t RECONNECT_INTERVAL_MS = 5000;
        static constexpr uint32_t MQTT_TIMEOUT_MS = 5000;
        static constexpr uint8_t MQTT_QOS = 1;

        bool attempt_connect();
        void publish_status(const char* status);
        void subscribe_to_commands();
        void handle_message(char* topic, byte* payload, unsigned int length);
        Command parse_command(const char* payload, unsigned int length);
        std::string build_data_topic() const;
        std::string build_command_topic() const;
        std::string build_status_topic() const;

        // Static callback wrapper for PubSubClient
        static void mqtt_callback_wrapper(char* topic, byte* payload, unsigned int length);
        static MQTTService* instance_;

    public:
        MQTTService();
        ~MQTTService() override;

        MQTTService(const MQTTService&) = delete;
        MQTTService& operator=(const MQTTService&) = delete;
        MQTTService(MQTTService&&) = delete;
        MQTTService& operator=(MQTTService&&) = delete;

        // IMQTTService interface
        common::patterns::Result<void> initialize(
            const std::string& device_id,
            const std::string& broker_host,
            uint16_t broker_port = 1883) override;

        void set_credentials(const std::string& username, const std::string& password) override;
        void set_enabled(bool enabled) override;
        void set_publish_interval(unsigned long intervalMs) override;
        void set_reading_callback(ReadingCallback callback) override;
        void set_command_callback(CommandCallback callback) override;
        void update() override;
        bool is_connected() const override;

        // Additional methods not in interface
        bool is_enabled() const { return enabled_; }
        const std::string& get_device_id() const { return device_id_; }
        common::patterns::Result<void> connect();
        void disconnect();
        common::patterns::Result<void> publish_reading(const SensorReading& reading);
        void force_send_reading();
        PubSubClient* get_client() { return &mqtt_client_; }
    };

} // namespace plant_nanny::services::mqtt
