#pragma once

#include "libs/common/patterns/Result.h"
#include "libs/common/logger/Logger.h"
#include "libs/common/service/Accessor.h"
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <string>
#include <functional>

namespace plant_nanny::services::mqtt
{
    /**
     * @brief Sensor reading data structure
     */
    struct SensorReading
    {
        float temperatureC;
        float humidityPct;
        float luminosityPct;
    };

    /**
     * @brief Command types received from server
     */
    enum class CommandType
    {
        Unknown,
        SendNow,        // Force immediate sensor reading
        PumpWater,      // Activate water pump
        SetInterval,    // Change publish interval
        Restart,        // Restart device
        OtaUpdate       // Trigger OTA update
    };

    /**
     * @brief Command received from server
     */
    struct Command
    {
        CommandType type;
        int durationMs;      // For pump_water
        int amountMl;        // For pump_water
        int intervalMs;      // For set_interval
        std::string otaUrl;  // For ota_update
    };

    /**
     * @brief MQTT Service for publishing sensor data to PlantNanny server
     *
     * Handles connection management, reconnection logic, and periodic publishing
     * of sensor readings to the MQTT broker.
     *
     * Topic structure (recommended architecture):
     *   devices/{deviceId}/data     - ESP32 → Server (telemetry, JSON)
     *   devices/{deviceId}/command  - Server → ESP32 (commands, JSON)
     *   devices/{deviceId}/status   - Device status (online/offline via LWT)
     *
     * Legacy topics (for backward compatibility):
     *   plantnanny/{deviceId}/sensors - Periodic sensor readings (JSON)
     *   plantnanny/{deviceId}/status  - Device status (online/offline)
     */
    class MQTTService
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

        bool use_new_topic_structure_;

        static constexpr uint32_t DEFAULT_PUBLISH_INTERVAL_MS = 60000;
        static constexpr uint32_t RECONNECT_INTERVAL_MS = 5000;
        static constexpr uint32_t MQTT_TIMEOUT_MS = 5000;
        static constexpr uint8_t MQTT_QOS = 1;

        bool attempt_connect();
        void publish_status(const char* status);
        void subscribe_to_commands();
        void handle_message(char* topic, byte* payload, unsigned int length);
        Command parse_command(const char* payload, unsigned int length);
        std::string build_topic(const char* suffix) const;
        std::string build_data_topic() const;
        std::string build_command_topic() const;
        std::string build_status_topic() const;

        // Static callback wrapper for PubSubClient
        static void mqtt_callback_wrapper(char* topic, byte* payload, unsigned int length);
        static MQTTService* instance_;

    public:
        MQTTService();
        ~MQTTService();

        MQTTService(const MQTTService&) = delete;
        MQTTService& operator=(const MQTTService&) = delete;
        MQTTService(MQTTService&&) = delete;
        MQTTService& operator=(MQTTService&&) = delete;

        /**
         * @brief Initialize the MQTT service
         * @param device_id Unique device identifier (MAC address recommended)
         * @param broker_host MQTT broker hostname or IP
         * @param broker_port MQTT broker port (default 1883)
         * @param use_new_topics Use new topic structure (devices/<id>/data) if true
         * @return Result indicating success or failure
         */
        common::patterns::Result<void> initialize(
            const std::string& device_id,
            const std::string& broker_host,
            uint16_t broker_port = 1883,
            bool use_new_topics = true);

        /**
         * @brief Set MQTT authentication credentials
         * @param username MQTT username
         * @param password MQTT password
         */
        void set_credentials(const std::string& username, const std::string& password);

        /**
         * @brief Set the callback to get current sensor readings
         * @param callback Function that returns current SensorReading
         */
        void set_reading_callback(ReadingCallback callback);

        /**
         * @brief Set the callback for received commands from server
         * @param callback Function called when command is received
         */
        void set_command_callback(CommandCallback callback);

        /**
         * @brief Set the publish interval for periodic updates
         * @param interval_ms Interval in milliseconds (minimum 1000ms)
         */
        void set_publish_interval(uint32_t interval_ms);

        /**
         * @brief Enable or disable MQTT publishing
         * @param enabled True to enable, false to disable
         */
        void set_enabled(bool enabled);

        /**
         * @brief Check if MQTT is enabled
         * @return True if enabled
         */
        bool is_enabled() const { return enabled_; }

        /**
         * @brief Check if connected to MQTT broker
         * @return True if connected
         */
        bool is_connected() const;

        /**
         * @brief Get the device ID
         * @return Device ID string
         */
        const std::string& get_device_id() const { return device_id_; }

        /**
         * @brief Connect to the MQTT broker
         * @return Result indicating success or failure
         */
        common::patterns::Result<void> connect();

        /**
         * @brief Disconnect from the MQTT broker
         */
        void disconnect();

        /**
         * @brief Publish current sensor reading immediately
         * @param reading Sensor reading to publish
         * @return Result indicating success or failure
         */
        common::patterns::Result<void> publish_reading(const SensorReading& reading);

        /**
         * @brief Force send sensor data immediately (triggered by send_now command)
         */
        void force_send_reading();

        /**
         * @brief Main update loop - call this regularly from app loop
         *
         * Handles:
         * - Connection maintenance and reconnection
         * - Periodic sensor reading publishing
         * - MQTT client loop processing (including incoming commands)
         */
        void update();

        /**
         * @brief Get the PubSubClient for external use (e.g., logging)
         * @return Pointer to the PubSubClient
         */
        PubSubClient* get_client() { return &mqtt_client_; }
    };

} // namespace plant_nanny::services::mqtt
