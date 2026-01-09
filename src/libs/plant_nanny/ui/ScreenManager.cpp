#include "libs/plant_nanny/ui/ScreenManager.h"

namespace plant_nanny::ui
{
    void ScreenManager::registerScreen(const std::string& id, std::shared_ptr<IScreen> screen)
    {
        _screens[id] = std::move(screen);
    }

    bool ScreenManager::navigateTo(const std::string& id)
    {
        auto it = _screens.find(id);
        if (it == _screens.end())
        {
            return false;
        }

        if (_currentScreen)
        {
            _currentScreen->onExit();
        }

        _currentScreen = it->second;
        _currentScreenId = id;
        _currentScreen->onEnter();
        _currentScreen->render();
        
        return true;
    }

    void ScreenManager::render()
    {
        if (_currentScreen)
        {
            _currentScreen->render();
        }
    }

} // namespace plant_nanny::ui
