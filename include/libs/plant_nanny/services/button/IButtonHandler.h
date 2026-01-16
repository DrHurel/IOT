#pragma once

#include "libs/common/patterns/Result.h"
#include <functional>

namespace plant_nanny::services::button
{
    enum class ButtonEvent;
    using ButtonCallback = std::function<void(ButtonEvent)>;

    /**
     * @brief Interface for button handling (DIP - Dependency Inversion Principle)
     */
    class IButtonHandler
    {
    public:
        virtual ~IButtonHandler() = default;

        virtual common::patterns::Result<void> initialize() = 0;
        virtual void setCallback(ButtonCallback callback) = 0;
        virtual ButtonEvent poll() = 0;
    };

} // namespace plant_nanny::services::button
