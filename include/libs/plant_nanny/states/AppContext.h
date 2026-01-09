#pragma once

#include "libs/plant_nanny/ui/ScreenManager.h"
#include "libs/plant_nanny/services/bluetooth/PairingManager.h"
#include "libs/plant_nanny/services/config/ConfigManager.h"
#include "libs/plant_nanny/ui/screens/PairingScreen.h"
#include <string>
#include <functional>

namespace plant_nanny
{
    /**
     * @brief Context passed to states - provides access to services (DIP)
     * 
     * States depend on this abstraction, not on concrete App.
     */
    class AppContext
    {
    public:
        virtual ~AppContext() = default;

        // Screen navigation
        virtual ui::ScreenManager& screenManager() = 0;
        
        // Services access (DIP - depend on abstractions)
        virtual services::bluetooth::PairingManager& pairingManager() = 0;
        virtual services::config::ConfigManager& configManager() = 0;
        
        // State data
        virtual void setCurrentPin(const std::string& pin) = 0;
        virtual const std::string& currentPin() const = 0;
        
        // Access to specific screens for updates
        virtual ui::screens::PairingScreen& pairingScreen() = 0;
        
        // Request state transition (decoupled from StateMachine)
        virtual void requestTransition(const std::string& stateId) = 0;
    };

} // namespace plant_nanny
