#include <libs/plant_nanny/App.h>
#include <libs/common/service/Registry.h>
#include <libs/common/logger/Logger.h>
#include <libs/common/logger/SerialLogger.h>
#include <libs/common/service/Accessor.h>
#include <libs/common/logger/MQTTLogger.h>
#include <libs/plant_nanny/services/network/Manager.h>
#include <libs/plant_nanny/services/network/INetworkService.h>
#include <libs/plant_nanny/services/ota/UpdateOrchestrator.h>

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
        Serial.begin(115200);
        while (!Serial && millis() < 5000)
        {
            ;
        }
        common::service::add<common::logger::Logger, common::logger::SerialLogger>();
#endif
#ifdef RELEASE
        common::service::add<common::logger::Logger, common::logger::MQTTLogger>(_mqtt_client.get());
#endif

        common::service::add<services::network::INetworkService, services::network::Manager>();

        auto network_service = common::service::get<services::network::INetworkService>();
        if (network_service.is_available())
        {
            common::service::add<services::ota::UpdateOrchestrator, services::ota::UpdateOrchestrator>();
        }

        auto logger = common::service::get<common::logger::Logger>();

        if (logger.is_available())
        {
            logger->info("App Initialized with logger, network, and OTA support");
        }
    }

    void App::run() const
    {
        auto network_service = common::service::get<services::network::INetworkService>();
        if (network_service.is_available())
        {
            network_service->maintain_connection();
        }

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

        auto network_service = common::service::get<services::network::INetworkService>();
        if (network_service.is_available())
        {
            network_service->disconnect();
        }
    }

    void App::configure_wifi(const std::string &ssid, const std::string &password)
    {
        auto network_service = common::service::get<services::network::INetworkService>();
        if (network_service.is_available())
        {
            network_service->set_credentials(ssid, password);
            auto result = network_service->connect();

            auto logger = common::service::get<common::logger::Logger>();
            if (result.succeed())
            {
                if (logger.is_available())
                {
                    logger->info("WiFi configured and connected");
                    auto ip = network_service->get_ip_address();
                    if (ip.succeed())
                    {
                        logger->info(("IP Address: " + ip.value()).c_str());
                    }
                }
            }
            else
            {
                if (logger.is_available())
                {
                    logger->error("WiFi connection failed");
                }
            }
        }
    }

    bool App::is_network_connected() const
    {
        auto network_service = common::service::get<services::network::INetworkService>();
        return network_service.is_available() && network_service->is_connected();
    }

    common::patterns::Result<void> App::perform_ota_update(const std::string &firmware_url)
    {
        auto logger = common::service::get<common::logger::Logger>();
        auto network_service = common::service::get<services::network::INetworkService>();

        if (!network_service.is_available() || !network_service->is_connected())
        {
            if (logger.is_available())
            {
                logger->error("OTA update failed: Network not connected");
            }
            return common::patterns::Result<void>::failure(
                common::patterns::Error("Network not connected"));
        }

        if (logger.is_available())
        {
            logger->info(("Starting OTA update from: " + firmware_url).c_str());
        }

        auto ota_orchestrator = common::service::get<services::ota::UpdateOrchestrator>();
        if (!ota_orchestrator.is_available())
        {
            if (logger.is_available())
            {
                logger->error("OTA orchestrator not available");
            }
            return common::patterns::Result<void>::failure(
                common::patterns::Error("OTA orchestrator not available"));
        }

        auto result = ota_orchestrator->update_from_url(firmware_url);

        if (result.succeed())
        {
            if (logger.is_available())
            {
                logger->info("OTA update successful - rebooting...");
            }
            delay(1000);
            ESP.restart();
        }
        else
        {
            if (logger.is_available())
            {
                logger->error("OTA update failed");
            }
        }

        return result;
    }
}
