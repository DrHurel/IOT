#pragma once

#include "IScreen.h"
#include <memory>
#include <unordered_map>
#include <string>

namespace plant_nanny::ui
{
    /**
     * @brief Screen manager - handles screen transitions (SRP)
     * 
     * Manages the lifecycle and rendering of screens.
     * Follows OCP - new screens can be added without modifying this class.
     */
    class ScreenManager
    {
    private:
        std::unordered_map<std::string, std::shared_ptr<IScreen>> _screens;
        std::shared_ptr<IScreen> _currentScreen;
        std::string _currentScreenId;

    public:
        ScreenManager() = default;
        ~ScreenManager() = default;
        
        ScreenManager(const ScreenManager&) = delete;
        ScreenManager& operator=(const ScreenManager&) = delete;
        ScreenManager(ScreenManager&&) = default;
        ScreenManager& operator=(ScreenManager&&) = default;

        /**
         * @brief Register a screen with an identifier
         * @param id Unique screen identifier
         * @param screen Screen instance
         */
        void registerScreen(const std::string& id, std::shared_ptr<IScreen> screen);

        /**
         * @brief Navigate to a screen by ID
         * @param id Screen identifier
         * @return true if navigation successful
         */
        bool navigateTo(const std::string& id);

        /**
         * @brief Get current screen ID
         */
        const std::string& currentScreenId() const { return _currentScreenId; }

        /**
         * @brief Render current screen
         */
        void render();
    };

} // namespace plant_nanny::ui
