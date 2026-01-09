#include "libs/plant_nanny/services/button/ButtonHandler.h"
#include <Arduino.h>

namespace plant_nanny::services::button
{
    ButtonHandler::ButtonHandler()
        : _callback(nullptr)
        , _leftPressStart(0)
        , _rightPressStart(0)
        , _leftWasPressed(false)
        , _rightWasPressed(false)
        , _leftLongPressTriggered(false)
        , _rightLongPressTriggered(false)
    {
    }

    common::patterns::Result<void> ButtonHandler::initialize()
    {
        pinMode(BUTTON_LEFT_PIN, INPUT_PULLUP);
        pinMode(BUTTON_RIGHT_PIN, INPUT_PULLUP);
        return common::patterns::Result<void>::success();
    }

    void ButtonHandler::setCallback(ButtonCallback callback)
    {
        _callback = callback;
    }

    bool ButtonHandler::isLeftPressed() const
    {
        return digitalRead(BUTTON_LEFT_PIN) == LOW;
    }

    bool ButtonHandler::isRightPressed() const
    {
        return digitalRead(BUTTON_RIGHT_PIN) == LOW;
    }

    unsigned long ButtonHandler::getLeftHoldDuration() const
    {
        if (_leftWasPressed && _leftPressStart > 0)
        {
            return millis() - _leftPressStart;
        }
        return 0;
    }

    unsigned long ButtonHandler::getRightHoldDuration() const
    {
        if (_rightWasPressed && _rightPressStart > 0)
        {
            return millis() - _rightPressStart;
        }
        return 0;
    }

    ButtonEvent ButtonHandler::poll()
    {
        ButtonEvent event = ButtonEvent::NONE;
        unsigned long now = millis();

        // Handle left button
        bool leftPressed = isLeftPressed();
        if (leftPressed && !_leftWasPressed)
        {
            // Button just pressed
            _leftPressStart = now;
            _leftLongPressTriggered = false;
        }
        else if (leftPressed && _leftWasPressed)
        {
            // Button held
            if (!_leftLongPressTriggered && (now - _leftPressStart) >= LONG_PRESS_DURATION_MS)
            {
                _leftLongPressTriggered = true;
                event = ButtonEvent::LEFT_LONG_PRESS;
                if (_callback) _callback(event);
            }
        }
        else if (!leftPressed && _leftWasPressed)
        {
            // Button just released
            if (!_leftLongPressTriggered && (now - _leftPressStart) >= DEBOUNCE_MS)
            {
                event = ButtonEvent::LEFT_SHORT_PRESS;
                if (_callback) _callback(event);
            }
            _leftPressStart = 0;
        }
        _leftWasPressed = leftPressed;

        // Handle right button
        bool rightPressed = isRightPressed();
        if (rightPressed && !_rightWasPressed)
        {
            // Button just pressed
            _rightPressStart = now;
            _rightLongPressTriggered = false;
        }
        else if (rightPressed && _rightWasPressed)
        {
            // Button held
            if (!_rightLongPressTriggered && (now - _rightPressStart) >= LONG_PRESS_DURATION_MS)
            {
                _rightLongPressTriggered = true;
                event = ButtonEvent::RIGHT_LONG_PRESS;
                if (_callback) _callback(event);
            }
        }
        else if (!rightPressed && _rightWasPressed)
        {
            // Button just released
            if (!_rightLongPressTriggered && (now - _rightPressStart) >= DEBOUNCE_MS)
            {
                event = ButtonEvent::RIGHT_SHORT_PRESS;
                if (_callback) _callback(event);
            }
            _rightPressStart = 0;
        }
        _rightWasPressed = rightPressed;

        return event;
    }

} // namespace plant_nanny::services::button
