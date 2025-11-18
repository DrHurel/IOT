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
#ifndef DEBUG
        common::service::add<common::logger::Logger, common::logger::SerialLogger>();
#endif
#ifndef RELEASE
        common::service::add<common::logger::Logger, common::logger::MQTTLogger>(_mqtt_client.get());
#endif


        auto logger = common::service::get<common::logger::Logger>();

        logger->info("App Started");
    }
}