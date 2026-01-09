#pragma once

#include <string>

namespace plant_nanny::ui
{
    /**
     * @brief Interface for all screens (ISP - Interface Segregation Principle)
     * 
     * Each screen is responsible for its own rendering logic.
     */
    class IScreen
    {
    public:
        virtual ~IScreen() = default;
        
        /**
         * @brief Render the screen
         */
        virtual void render() = 0;
        
        /**
         * @brief Called when the screen becomes active
         */
        virtual void onEnter() {}
        
        /**
         * @brief Called when the screen becomes inactive
         */
        virtual void onExit() {}
    };

} // namespace plant_nanny::ui
