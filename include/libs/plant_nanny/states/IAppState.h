#pragma once

#include "libs/plant_nanny/services/button/ButtonHandler.h"

namespace plant_nanny
{
    // Forward declaration
    class AppContext;

    /**
     * @brief Interface for application states (State Pattern - OCP)
     * 
     * New states can be added without modifying existing code.
     * Each state handles its own button events and transitions.
     */
    class IAppState
    {
    public:
        virtual ~IAppState() = default;

        /**
         * @brief Called when entering this state
         */
        virtual void onEnter(AppContext& context) = 0;

        /**
         * @brief Called when exiting this state
         */
        virtual void onExit(AppContext& context) = 0;

        /**
         * @brief Handle button events
         * @return Next state name, or empty string to stay in current state
         */
        virtual std::string handleButton(AppContext& context, services::button::ButtonEvent event) = 0;

        /**
         * @brief Called every frame for state-specific updates
         */
        virtual void update(AppContext& context) = 0;

        /**
         * @brief Get state identifier
         */
        virtual const char* id() const = 0;
    };

} // namespace plant_nanny
