#include <libs/plant_nanny/App.h>
#include <libs/common/service/Registry.h>
#include <libs/common/logger/Logger.h>
#include <libs/common/logger/SerialLogger.h>
#include <libs/common/service/Accessor.h>
#include <libs/common/logger/MQTTLogger.h>
namespace plant_nanny
{

    App::App()
    {
        _mqtt_client = std::make_unique<PubSubClient>();
    }

    void App::initialize()
    {
        common::service::DefaultRegistry::create();
#ifdef DEBUG
        // Initialize Serial communication
        Serial.begin(9600);
        while (!Serial && millis() < 5000)
        {
            ; // Wait for serial port to connect or timeout after 5 seconds
        }
        // In DEBUG mode, use SerialLogger
        common::service::add<common::logger::Logger, common::logger::SerialLogger>();
#endif
#ifdef RELEASE
        // In RELEASE mode, use MQTTLogger
        common::service::add<common::logger::Logger, common::logger::MQTTLogger>(_mqtt_client.get());
#endif

        auto logger = common::service::get<common::logger::Logger>();

        if (logger.is_available())
        {
            logger->info("App Initialized with logger");
        }
        else
        {
            Serial.println("Warning: Logger is null!");
        }
    }

    void App::run() const
    {
        // Main application loop
        auto logger = common::service::get<common::logger::Logger>();
        if (logger.is_available())
        {
            logger->debug("App running");
        }
    }

    void App::shutdown()
    {
        auto logger = common::service::get<common::logger::Logger>();
        if (logger.is_available())
        {
            logger->info("App shutting down");
        }
    }
}