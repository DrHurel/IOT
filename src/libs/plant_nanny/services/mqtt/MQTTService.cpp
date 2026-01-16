#include "libs/plant_nanny/services/mqtt/MQTTService.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include "libs/common/logger/Log.h"

namespace plant_nanny::services::mqtt
{
    // Static instance for callback wrapper
    MQTTService* MQTTService::instance_ = nullptr;

    MQTTService::MQTTService()
        : mqtt_client_(wifi_client_)
        , broker_port_(1883)
        , initialized_(false)
        , enabled_(false)
        , last_publish_time_(0)
        , last_reconnect_attempt_(0)
        , publish_interval_ms_(DEFAULT_PUBLISH_INTERVAL_MS)
        , reading_callback_(nullptr)
        , command_callback_(nullptr)
    {
        instance_ = this;
    }

    MQTTService::~MQTTService()
    {
        if (is_connected())
        {
            publish_status("offline");
            mqtt_client_.disconnect();
        }
        instance_ = nullptr;
    }

    void MQTTService::mqtt_callback_wrapper(char* topic, byte* payload, unsigned int length)
    {
        if (instance_)
        {
            instance_->handle_message(topic, payload, length);
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
        mqtt_client_.setCallback(mqtt_callback_wrapper);
        mqtt_client_.setBufferSize(512);

        initialized_ = true;
        
        char msg[128];
        snprintf(msg, sizeof(msg), "[MQTT] Service initialized - Device: %s, Broker: %s:%u", 
                 device_id_.c_str(), broker_host_.c_str(), broker_port_);
        LOG_INFO(msg);

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

    void MQTTService::set_command_callback(CommandCallback callback)
    {
        command_callback_ = callback;
    }

    void MQTTService::set_publish_interval(unsigned long interval_ms)
    {
        publish_interval_ms_ = (interval_ms < 1000) ? 1000 : interval_ms;
    }

    void MQTTService::set_enabled(bool enabled)
    {
        if (enabled_ != enabled)
        {
            enabled_ = enabled;
            if (enabled)
            {
                LOG_INFO("[MQTT] Publishing enabled");
            }
            else
            {
                LOG_INFO("[MQTT] Publishing disabled");
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

    std::string MQTTService::build_data_topic() const
    {
        return "devices/" + device_id_ + "/data";
    }

    std::string MQTTService::build_command_topic() const
    {
        return "devices/" + device_id_ + "/command";
    }

    std::string MQTTService::build_status_topic() const
    {
        return "devices/" + device_id_ + "/status";
    }

    void MQTTService::subscribe_to_commands()
    {
        std::string command_topic = build_command_topic();
        
        if (mqtt_client_.subscribe(command_topic.c_str(), MQTT_QOS))
        {
            char msg[128];
            snprintf(msg, sizeof(msg), "[MQTT] Subscribed to: %s", command_topic.c_str());
            LOG_INFO(msg);
        }
        else
        {
            LOG_ERROR("[MQTT] Failed to subscribe to command topic");
        }
    }

    void MQTTService::handle_message(char* topic, byte* payload, unsigned int length)
    {
        char msg[128];
        snprintf(msg, sizeof(msg), "[MQTT] Message received on: %s (%u bytes)", topic, length);
        LOG_DEBUG(msg);

        std::string topic_str(topic);
        std::string expected_command_topic = build_command_topic();

        if (topic_str != expected_command_topic)
        {
            snprintf(msg, sizeof(msg), "[MQTT] Ignoring message on unexpected topic: %s", topic);
            LOG_DEBUG(msg);
            return;
        }

        Command cmd = parse_command(reinterpret_cast<char*>(payload), length);

        if (cmd.type != CommandType::Unknown && command_callback_)
        {
            command_callback_(cmd);
        }
        else if (cmd.type == CommandType::SendNow)
        {
            force_send_reading();
        }
    }

    Command MQTTService::parse_command(const char* payload, unsigned int length)
    {
        Command cmd;
        cmd.type = CommandType::Unknown;
        cmd.durationMs = 0;
        cmd.amountMl = 0;
        cmd.intervalMs = 0;

        StaticJsonDocument<256> doc;
        DeserializationError error = deserializeJson(doc, payload, length);

        if (error)
        {
            char msg[64];
            snprintf(msg, sizeof(msg), "[MQTT] Failed to parse command JSON: %s", error.c_str());
            LOG_ERROR(msg);
            return cmd;
        }

        const char* action = doc["action"] | "";

        if (strcmp(action, "send_now") == 0)
        {
            cmd.type = CommandType::SendNow;
            LOG_INFO("[MQTT] Received command: send_now");
        }
        else if (strcmp(action, "pump_water") == 0)
        {
            cmd.type = CommandType::PumpWater;
            cmd.durationMs = doc["durationMs"] | 0;
            cmd.amountMl = doc["amountMl"] | 0;
            
            char msg[64];
            snprintf(msg, sizeof(msg), "[MQTT] Received command: pump_water (%dms, %dml)", 
                     cmd.durationMs, cmd.amountMl);
            LOG_INFO(msg);
        }
        else if (strcmp(action, "set_interval") == 0)
        {
            cmd.type = CommandType::SetInterval;
            cmd.intervalMs = doc["intervalMs"] | 60000;
            
            char msg[64];
            snprintf(msg, sizeof(msg), "[MQTT] Received command: set_interval (%dms)", cmd.intervalMs);
            LOG_INFO(msg);
            
            // Apply interval change directly
            set_publish_interval(cmd.intervalMs);
        }
        else if (strcmp(action, "restart") == 0)
        {
            cmd.type = CommandType::Restart;
            LOG_INFO("[MQTT] Received command: restart");
        }
        else if (strcmp(action, "ota_update") == 0)
        {
            cmd.type = CommandType::OtaUpdate;
            cmd.otaUrl = doc["url"] | "";
            LOG_INFO("[MQTT] Received command: ota_update");
        }
        else
        {
            char msg[64];
            snprintf(msg, sizeof(msg), "[MQTT] Unknown command action: %s", action);
            LOG_ERROR(msg);
        }

        return cmd;
    }

    bool MQTTService::attempt_connect()
    {
        if (!initialized_)
        {
            return false;
        }

        LOG_INFO("[MQTT] Attempting connection...");
        
        // Debug: Print connection details
        char debug_msg[256];
        snprintf(debug_msg, sizeof(debug_msg), "[MQTT] Connecting to %s:%u as %s (user: %s)", 
                 broker_host_.c_str(), broker_port_, device_id_.c_str(),
                 username_.empty() ? "(none)" : username_.c_str());
        Serial.println(debug_msg);

        bool connected = false;
        std::string client_id = "plantnanny-" + device_id_;
        std::string will_topic = build_status_topic();
        const char* will_message = "{\"status\":\"offline\"}";

        if (username_.empty())
        {
            connected = mqtt_client_.connect(
                client_id.c_str(),
                will_topic.c_str(),
                MQTT_QOS,
                true,
                will_message
            );
        }
        else
        {
            connected = mqtt_client_.connect(
                client_id.c_str(),
                username_.c_str(),
                password_.c_str(),
                will_topic.c_str(),
                MQTT_QOS,
                true,
                will_message
            );
        }

        if (connected)
        {
            LOG_INFO("[MQTT] Connected to broker");
            publish_status("online");
            subscribe_to_commands();
            return true;
        }
        else
        {
            char error_msg[64];
            snprintf(error_msg, sizeof(error_msg), "[MQTT] Connection failed, rc=%d", mqtt_client_.state());
            LOG_ERROR(error_msg);
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
            LOG_INFO("[MQTT] Disconnected");
        }
    }

    void MQTTService::publish_status(const char* status)
    {
        std::string topic = build_status_topic();
        StaticJsonDocument<64> doc;
        doc["status"] = status;
        
        char payload[64];
        serializeJson(doc, payload, sizeof(payload));
        mqtt_client_.publish(topic.c_str(), payload, true);
    }

    common::patterns::Result<void> MQTTService::publish_reading(const SensorReading& reading)
    {
        if (!is_connected())
        {
            return common::patterns::Result<void>::failure(
                common::patterns::Error("Not connected to MQTT broker"));
        }

        StaticJsonDocument<256> doc;
        doc["temperatureC"] = reading.temperatureC;
        doc["humidityPct"] = reading.humidityPct;
        doc["luminosityPct"] = reading.luminosityPct;
        doc["ts"] = static_cast<unsigned long>(time(nullptr));
        doc["uptime"] = millis() / 1000;

        char payload[256];
        size_t len = serializeJson(doc, payload, sizeof(payload));

        std::string topic = build_data_topic();

        if (mqtt_client_.publish(topic.c_str(), reinterpret_cast<const uint8_t*>(payload), len, false))
        {
            LOG_INFO("[MQTT] Sensor reading published");
            return common::patterns::Result<void>::success();
        }

        return common::patterns::Result<void>::failure(
            common::patterns::Error("Failed to publish sensor reading"));
    }

    void MQTTService::force_send_reading()
    {
        if (reading_callback_)
        {
            LOG_INFO("[MQTT] Force sending sensor reading");
            SensorReading reading = reading_callback_();
            publish_reading(reading);
            last_publish_time_ = millis();
        }
    }

    void MQTTService::update()
    {
        if (!initialized_ || !enabled_)
        {
            return;
        }

        uint32_t now = millis();

        if (!is_connected())
        {
            if (now - last_reconnect_attempt_ >= RECONNECT_INTERVAL_MS)
            {
                last_reconnect_attempt_ = now;
                attempt_connect();
            }
            return;
        }

        mqtt_client_.loop();

        if (reading_callback_ && (now - last_publish_time_ >= publish_interval_ms_))
        {
            last_publish_time_ = now;

            SensorReading reading = reading_callback_();
            publish_reading(reading);
        }
    }

} // namespace plant_nanny::services::mqtt
