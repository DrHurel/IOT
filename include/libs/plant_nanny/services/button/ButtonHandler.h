#pragma once

#include "libs/plant_nanny/services/button/IButtonHandler.h"
#include "libs/common/patterns/Result.h"
#include <functional>
#include <cstdint>

#define BUTTON_LEFT_PIN 0
#define BUTTON_RIGHT_PIN 35

namespace plant_nanny::services::button
{
    enum class ButtonEvent
    {
        NONE,
        LEFT_SHORT_PRESS,
        LEFT_LONG_PRESS,
        RIGHT_SHORT_PRESS,
        RIGHT_LONG_PRESS
    };

    using ButtonCallback = std::function<void(ButtonEvent)>;

    class ButtonHandler : public IButtonHandler
    {
    private:
        ButtonCallback _callback;
        
        unsigned long _leftPressStart;
        unsigned long _rightPressStart;
        bool _leftWasPressed;
        bool _rightWasPressed;
        bool _leftLongPressTriggered;
        bool _rightLongPressTriggered;
        
        static constexpr unsigned long LONG_PRESS_DURATION_MS = 3000;  // 3 seconds
        static constexpr unsigned long DEBOUNCE_MS = 50;

    public:
        ButtonHandler();
        ~ButtonHandler() override = default;
        
        ButtonHandler(const ButtonHandler &) = delete;
        ButtonHandler &operator=(const ButtonHandler &) = delete;
        ButtonHandler(ButtonHandler &&) = delete;
        ButtonHandler &operator=(ButtonHandler &&) = delete;

        common::patterns::Result<void> initialize() override;
        void setCallback(ButtonCallback callback) override;
        ButtonEvent poll() override;

        /**
         * @brief Check if left button is currently pressed
         */
        bool isLeftPressed() const;

        /**
         * @brief Check if right button is currently pressed
         */
        bool isRightPressed() const;

        /**
         * @brief Get how long left button has been held (in ms)
         */
        unsigned long getLeftHoldDuration() const;

        /**
         * @brief Get how long right button has been held (in ms)
         */
        unsigned long getRightHoldDuration() const;
    };

} // namespace plant_nanny::services::button
