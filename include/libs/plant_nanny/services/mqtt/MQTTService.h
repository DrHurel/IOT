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
     * @brief MQTT Service for publishing sensor data to PlantNanny server
     *
     * Handles connection management, reconnection logic, and periodic publishing
     * of sensor readings to the MQTT broker.
     *
     * Topics used:
     *   plantnanny/{deviceId}/sensors - Periodic sensor readings (JSON)
     *   plantnanny/{deviceId}/status  - Device status (online/offline)
     */
    class MQTTService
    {
    public:
        using ReadingCallback = std::function<SensorReading()>;

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

        static constexpr uint32_t DEFAULT_PUBLISH_INTERVAL_MS = 60000;  // 1 minute
        static constexpr uint32_t RECONNECT_INTERVAL_MS = 5000;         // 5 seconds
        static constexpr uint32_t MQTT_TIMEOUT_MS = 5000;               // 5 seconds

        bool attempt_connect();
        void publish_status(const char* status);
        std::string build_topic(const char* suffix) const;

    public:
        MQTTService();
        ~MQTTService();

        MQTTService(const MQTTService&) = delete;
        MQTTService& operator=(const MQTTService&) = delete;
        MQTTService(MQTTService&&) = delete;
        MQTTService& operator=(MQTTService&&) = delete;

        /**
         * @brief Initialize the MQTT service
         * @param device_id Unique device identifier
         * @param broker_host MQTT broker hostname or IP
         * @param broker_port MQTT broker port (default 1883)
         * @return Result indicating success or failure
         */
        common::patterns::Result<void> initialize(
            const std::string& device_id,
            const std::string& broker_host,
            uint16_t broker_port = 1883);

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
         * @brief Main update loop - call this regularly from app loop
         *
         * Handles:
         * - Connection maintenance and reconnection
         * - Periodic sensor reading publishing
         * - MQTT client loop processing
         */
        void update();

        /**
         * @brief Get the PubSubClient for external use (e.g., logging)
         * @return Pointer to the PubSubClient
         */
        PubSubClient* get_client() { return &mqtt_client_; }
    };

} // namespace plant_nanny::services::mqtt
