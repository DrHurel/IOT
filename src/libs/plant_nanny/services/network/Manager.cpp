#include "libs/plant_nanny/services/network/Manager.h"
#include "libs/common/utils/LogMacros.h"
#include <WiFi.h>
#include <HTTPClient.h>

namespace plant_nanny::services::network
{
    Manager::Manager() 
        : logger_(common::service::get<common::logger::Logger>())
        , connected_(false)
        , last_connection_attempt_(0)
    {
        LOG_IF_AVAILABLE(logger_, info, "Network Manager initialized");
    }

    void Manager::set_credentials(const std::string& ssid, const std::string& password)
    {
        ssid_ = ssid;
        password_ = password;
        LOG_IF_AVAILABLE(logger_, info, "WiFi credentials configured");
    }

    bool Manager::attempt_connection()
    {
        if (ssid_.empty())
        {
            LOG_IF_AVAILABLE(logger_, error, "WiFi credentials not set");
            return false;
        }

        LOG_IF_AVAILABLE(logger_, info, "Attempting WiFi connection...");
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid_.c_str(), password_.c_str());

        uint32_t start_time = millis();
        while (WiFi.status() != WL_CONNECTED && (millis() - start_time) < 10000)
        {
            delay(500);
        }

        if (WiFi.status() == WL_CONNECTED)
        {
            LOG_IF_AVAILABLE(logger_, info, "WiFi connected");
            connected_ = true;
            return true;
        }
        else
        {
            LOG_IF_AVAILABLE(logger_, error, "WiFi connection failed");
            connected_ = false;
            return false;
        }
    }

    common::patterns::Result<void> Manager::connect()
    {
        if (attempt_connection())
        {
            return common::patterns::Result<void>::success();
        }
        return common::patterns::Result<void>::failure(
            common::patterns::Error("Failed to connect to WiFi")
        );
    }

    void Manager::disconnect()
    {
        WiFi.disconnect();
        LOG_IF_AVAILABLE(logger_, info, "WiFi disconnected");
        connected_ = false;
    }

    bool Manager::is_connected() const
    {
        return WiFi.status() == WL_CONNECTED;
    }

    void Manager::maintain_connection()
    {
        uint32_t current_time = millis();
        
        if (!is_connected() && 
            (current_time - last_connection_attempt_) >= RECONNECT_INTERVAL_MS)
        {
            last_connection_attempt_ = current_time;
            LOG_IF_AVAILABLE(logger_, info, "Attempting to reconnect...");
            attempt_connection();
        }
    }

    common::patterns::Result<std::string> Manager::get_ip_address() const
    {
        if (!is_connected())
        {
            return common::patterns::Result<std::string>::failure(
                common::patterns::Error("Not connected to WiFi")
            );
        }
        return common::patterns::Result<std::string>::success(WiFi.localIP().toString().c_str());
    }

    common::patterns::Result<int> Manager::get_rssi() const
    {
        if (!is_connected())
        {
            return common::patterns::Result<int>::failure(
                common::patterns::Error("Not connected to WiFi")
            );
        }
        return common::patterns::Result<int>::success(WiFi.RSSI());
    }

    common::patterns::Result<void> Manager::download_file(
        const std::string& url,
        std::function<bool(const uint8_t*, size_t)> chunk_handler,
        std::function<void(size_t, size_t)> progress_callback)
    {
        if (!is_connected())
        {
            return common::patterns::Result<void>::failure(
                common::patterns::Error("Not connected to WiFi")
            );
        }

        HTTPClient http;
        http.begin(url.c_str());
        
        int httpCode = http.GET();
        if (httpCode != HTTP_CODE_OK)
        {
            http.end();
            LOG_IF_AVAILABLE(logger_, error, "HTTP GET failed");
            return common::patterns::Result<void>::failure(
                common::patterns::Error("HTTP GET failed with code: " + std::to_string(httpCode))
            );
        }

        int total_length = http.getSize();
        size_t downloaded = 0;

        WiFiClient* stream = http.getStreamPtr();
        uint8_t buffer[512];

        LOG_IF_AVAILABLE(logger_, info, "Starting download...");

        while (http.connected() && (total_length > 0 || total_length == -1))
        {
            size_t available = stream->available();
            if (available)
            {
                size_t read_len = stream->readBytes(buffer, std::min(available, sizeof(buffer)));
                
                if (!chunk_handler(buffer, read_len))
                {
                    http.end();
                    LOG_IF_AVAILABLE(logger_, error, "Chunk handler failed");
                    return common::patterns::Result<void>::failure(
                        common::patterns::Error("Chunk handler returned false")
                    );
                }

                downloaded += read_len;
                
                if (progress_callback)
                {
                    progress_callback(downloaded, total_length);
                }

                if (total_length > 0 && downloaded >= static_cast<size_t>(total_length))
                {
                    break;
                }
            }
            delay(1);
        }

        http.end();
        LOG_IF_AVAILABLE(logger_, info, "Download completed");
        return common::patterns::Result<void>::success();
    }

} // namespace plant_nanny::services::network