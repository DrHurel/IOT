#include "libs/plant_nanny/services/mqtt/MQTTService.h"
#include <Arduino.h>
#include <ArduinoJson.h>

namespace
{
    void log_info(const char* message)
    {
        auto logger = common::service::get<common::logger::Logger>();
        if (logger.is_available())
        {
            logger->info(message);
        }
    }

    void log_error(const char* message)
    {
        auto logger = common::service::get<common::logger::Logger>();
        if (logger.is_available())
        {
            logger->error(message);
        }
    }
}

namespace plant_nanny::services::mqtt
{
    MQTTService::MQTTService()
        : mqtt_client_(wifi_client_)
        , broker_port_(1883)
        , initialized_(false)
        , enabled_(false)
        , last_publish_time_(0)
        , last_reconnect_attempt_(0)
        , publish_interval_ms_(DEFAULT_PUBLISH_INTERVAL_MS)
        , reading_callback_(nullptr)
    {
    }

    MQTTService::~MQTTService()
    {
        if (is_connected())
        {
            publish_status("offline");
            mqtt_client_.disconnect();
        }
    }

    common::patterns::Result<void> MQTTService::initialize(
        const std::string& device_id,
        const std::string& broker_host,
        uint16_t broker_port)
    {
        if (device_id.empty())
        {
            return common::patterns::Result<void>::failure(
                common::patterns::Error("Device ID cannot be empty"));
        }

        if (broker_host.empty())
        {
            return common::patterns::Result<void>::failure(
                common::patterns::Error("Broker host cannot be empty"));
        }

        device_id_ = device_id;
        broker_host_ = broker_host;
        broker_port_ = broker_port;

        mqtt_client_.setServer(broker_host_.c_str(), broker_port_);
        mqtt_client_.setKeepAlive(60);
        mqtt_client_.setSocketTimeout(MQTT_TIMEOUT_MS / 1000);

        initialized_ = true;
        log_info("[MQTT] Service initialized");

        return common::patterns::Result<void>::success();
    }

    void MQTTService::set_credentials(const std::string& username, const std::string& password)
    {
        username_ = username;
        password_ = password;
    }

    void MQTTService::set_reading_callback(ReadingCallback callback)
    {
        reading_callback_ = callback;
    }

    void MQTTService::set_publish_interval(uint32_t interval_ms)
    {
        // Minimum 1 second interval
        publish_interval_ms_ = (interval_ms < 1000) ? 1000 : interval_ms;
    }

    void MQTTService::set_enabled(bool enabled)
    {
        if (enabled_ != enabled)
        {
            enabled_ = enabled;
            if (enabled)
            {
                log_info("[MQTT] Publishing enabled");
            }
            else
            {
                log_info("[MQTT] Publishing disabled");
                if (is_connected())
                {
                    publish_status("offline");
                    mqtt_client_.disconnect();
                }
            }
        }
    }

    bool MQTTService::is_connected() const
    {
        // PubSubClient::connected() is not const, so we need to cast
        return const_cast<PubSubClient&>(mqtt_client_).connected();
    }

    std::string MQTTService::build_topic(const char* suffix) const
    {
        return "plantnanny/" + device_id_ + "/" + suffix;
    }

    bool MQTTService::attempt_connect()
    {
        if (!initialized_)
        {
            return false;
        }

        log_info("[MQTT] Attempting connection...");

        bool connected = false;
        std::string client_id = "plantnanny-" + device_id_;
        std::string will_topic = build_topic("status");

        if (username_.empty())
        {
            connected = mqtt_client_.connect(
                client_id.c_str(),
                will_topic.c_str(),  // Will topic
                0,                    // Will QoS
                true,                 // Will retain
                "offline"             // Will message
            );
        }
        else
        {
            connected = mqtt_client_.connect(
                client_id.c_str(),
                username_.c_str(),
                password_.c_str(),
                will_topic.c_str(),  // Will topic
                0,                    // Will QoS
                true,                 // Will retain
                "offline"             // Will message
            );
        }

        if (connected)
        {
            log_info("[MQTT] Connected to broker");
            publish_status("online");
            return true;
        }
        else
        {
            char error_msg[64];
            snprintf(error_msg, sizeof(error_msg), "[MQTT] Connection failed, rc=%d", mqtt_client_.state());
            log_error(error_msg);
            return false;
        }
    }

    common::patterns::Result<void> MQTTService::connect()
    {
        if (!initialized_)
        {
            return common::patterns::Result<void>::failure(
                common::patterns::Error("MQTT service not initialized"));
        }

        if (is_connected())
        {
            return common::patterns::Result<void>::success();
        }

        if (attempt_connect())
        {
            return common::patterns::Result<void>::success();
        }

        return common::patterns::Result<void>::failure(
            common::patterns::Error("Failed to connect to MQTT broker"));
    }

    void MQTTService::disconnect()
    {
        if (is_connected())
        {
            publish_status("offline");
            mqtt_client_.disconnect();
            log_info("[MQTT] Disconnected");
        }
    }

    void MQTTService::publish_status(const char* status)
    {
        std::string topic = build_topic("status");
        mqtt_client_.publish(topic.c_str(), status, true);  // Retained
    }

    common::patterns::Result<void> MQTTService::publish_reading(const SensorReading& reading)
    {
        if (!is_connected())
        {
            return common::patterns::Result<void>::failure(
                common::patterns::Error("Not connected to MQTT broker"));
        }

        // Build JSON payload
        StaticJsonDocument<256> doc;
        doc["temperatureC"] = reading.temperatureC;
        doc["humidityPct"] = reading.humidityPct;
        doc["luminosityPct"] = reading.luminosityPct;

        // Add timestamp (Unix timestamp in seconds)
        doc["ts"] = static_cast<unsigned long>(time(nullptr));

        char payload[256];
        size_t len = serializeJson(doc, payload, sizeof(payload));

        std::string topic = build_topic("sensors");

        if (mqtt_client_.publish(topic.c_str(), payload, len))
        {
            log_info("[MQTT] Sensor reading published");
            return common::patterns::Result<void>::success();
        }

        return common::patterns::Result<void>::failure(
            common::patterns::Error("Failed to publish sensor reading"));
    }

    void MQTTService::update()
    {
        if (!initialized_ || !enabled_)
        {
            return;
        }

        uint32_t now = millis();

        // Maintain connection
        if (!is_connected())
        {
            // Attempt reconnection with backoff
            if (now - last_reconnect_attempt_ >= RECONNECT_INTERVAL_MS)
            {
                last_reconnect_attempt_ = now;
                attempt_connect();
            }
            return;
        }

        // Process MQTT client loop
        mqtt_client_.loop();

        // Periodic publishing
        if (reading_callback_ && (now - last_publish_time_ >= publish_interval_ms_))
        {
            last_publish_time_ = now;

            SensorReading reading = reading_callback_();
            publish_reading(reading);
        }
    }

} // namespace plant_nanny::services::mqtt
