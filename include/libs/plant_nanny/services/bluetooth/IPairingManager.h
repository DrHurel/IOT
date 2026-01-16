#pragma once

#include "libs/common/patterns/Result.h"
#include <string>
#include <functional>

namespace plant_nanny::services::bluetooth
{
    enum class PairingState;
    struct WifiCredentials;
    struct MqttConfig;

    using PinDisplayCallback = std::function<void(const std::string& pin)>;
    using PairingCompleteCallback = std::function<void(bool success)>;
    using WifiConfigCallback = std::function<void(const WifiCredentials& creds)>;
    using MqttConfigCallback = std::function<void(const MqttConfig& config)>;
    using StateChangeCallback = std::function<void(PairingState newState)>;

    /**
     * @brief Interface for Bluetooth pairing management (DIP - Dependency Inversion Principle)
     */
    class IPairingManager
    {
    public:
        virtual ~IPairingManager() = default;

        virtual common::patterns::Result<void> initialize() = 0;
        virtual common::patterns::Result<void> startPairing() = 0;
        virtual common::patterns::Result<void> stopPairing() = 0;
        virtual bool verifyPin(const std::string& pin) = 0;
        virtual PairingState getState() const = 0;
        virtual void setState(PairingState state) = 0;
        virtual void setDeviceId(const std::string& deviceId) = 0;
        virtual void setIpAddress(const std::string& ipAddress) = 0;
        virtual void notifyWifiConfigured(bool success) = 0;

        // Callbacks
        virtual void setPinDisplayCallback(PinDisplayCallback callback) = 0;
        virtual void setPairingCompleteCallback(PairingCompleteCallback callback) = 0;
        virtual void setWifiConfigCallback(WifiConfigCallback callback) = 0;
        virtual void setMqttConfigCallback(MqttConfigCallback callback) = 0;
        virtual void setStateChangeCallback(StateChangeCallback callback) = 0;

        // Additional methods
        virtual void update() = 0;
        virtual const std::string& getCurrentPin() const = 0;
        virtual common::patterns::Result<void> unpair() = 0;
    };

} // namespace plant_nanny::services::bluetooth
