#pragma once

#include "libs/common/patterns/Result.h"
#include <functional>
#include <cstdint>

// LilyGo T-Display button pins
#define BUTTON_LEFT_PIN 0    // GPIO 0 - Boot button (left)
#define BUTTON_RIGHT_PIN 35  // GPIO 35 (right)

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

    class ButtonHandler
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
        ~ButtonHandler() = default;
        
        ButtonHandler(const ButtonHandler &) = delete;
        ButtonHandler &operator=(const ButtonHandler &) = delete;
        ButtonHandler(ButtonHandler &&) = delete;
        ButtonHandler &operator=(ButtonHandler &&) = delete;

        /**
         * @brief Initialize button GPIOs
         */
        common::patterns::Result<void> initialize();

        /**
         * @brief Set callback for button events
         */
        void setCallback(ButtonCallback callback);

        /**
         * @brief Poll buttons - call this in the main loop
         * @return ButtonEvent if any button event occurred
         */
        ButtonEvent poll();

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
