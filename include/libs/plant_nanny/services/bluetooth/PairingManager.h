#pragma once

#include "libs/common/patterns/Result.h"
#include "libs/common/logger/Logger.h"
#include "libs/common/service/Accessor.h"
#include <functional>
#include <string>
#include <cstdint>

namespace plant_nanny::services::bluetooth
{
    enum class PairingState
    {
        IDLE,
        ADVERTISING,
        AWAITING_PIN,
        CONNECTED,
        PAIRED,
        FAILED
    };

    using PinDisplayCallback = std::function<void(const std::string& pin)>;
    using PairingCompleteCallback = std::function<void(bool success)>;

    class PairingManager
    {
    private:
        PairingState _state;
        std::string _currentPin;
        PinDisplayCallback _pinDisplayCallback;
        PairingCompleteCallback _pairingCompleteCallback;
        bool _initialized;
        unsigned long _pairingStartTime;
        static constexpr unsigned long PAIRING_TIMEOUT_MS = 60000;  // 60 seconds timeout

        /**
         * @brief Generate a random 4-digit PIN
         */
        std::string generatePin();

    public:
        static constexpr const char* DEVICE_NAME = "PlantNanny";

        PairingManager();
        ~PairingManager();

        PairingManager(const PairingManager&) = delete;
        PairingManager& operator=(const PairingManager&) = delete;
        PairingManager(PairingManager&&) = delete;
        PairingManager& operator=(PairingManager&&) = delete;

        /**
         * @brief Initialize the Bluetooth stack for pairing
         */
        common::patterns::Result<void> initialize();

        /**
         * @brief Start pairing mode - will generate PIN and start advertising
         * @return Result with the generated PIN or error
         */
        common::patterns::Result<std::string> startPairing();

        /**
         * @brief Stop pairing mode
         */
        common::patterns::Result<void> stopPairing();

        /**
         * @brief Check and update pairing status - call in main loop
         */
        void update();

        /**
         * @brief Get current pairing state
         */
        PairingState getState() const { return _state; }

        /**
         * @brief Get current PIN (only valid during pairing)
         */
        const std::string& getCurrentPin() const { return _currentPin; }

        /**
         * @brief Check if a device is currently paired
         */
        bool isPaired() const { return _state == PairingState::PAIRED; }

        /**
         * @brief Set callback for PIN display
         */
        void setPinDisplayCallback(PinDisplayCallback callback) { _pinDisplayCallback = callback; }

        /**
         * @brief Set callback for pairing completion
         */
        void setPairingCompleteCallback(PairingCompleteCallback callback) { _pairingCompleteCallback = callback; }

        /**
         * @brief Disconnect and unpair current device
         */
        common::patterns::Result<void> unpair();
    };

} // namespace plant_nanny::services::bluetooth
