#pragma once

namespace plant_nanny::services::pump
{
    /**
     * @brief Interface for pump control (DIP - Dependency Inversion Principle)
     */
    class IPump
    {
    public:
        virtual ~IPump() = default;

        virtual void activate() = 0;
        virtual void deactivate() = 0;
        virtual void toggle() = 0;
        virtual bool isActive() const = 0;
        virtual void setActive(bool active) = 0;
    };

} // namespace plant_nanny::services::pump
