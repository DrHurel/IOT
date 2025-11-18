#include <libs/plant_nanny/App.h>


namespace plant_nanny {

    App::App() {
        _mqtt_client = std::make_unique<PubSubClient>();
    }

    void App::initialize() {
        
    }
}