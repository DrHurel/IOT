#pragma once

#include <libs/common/App.h>
#include <memory>
#include <PubSubClient.h>
#include "libs/common/patterns/Result.h"
#include <string>

namespace plant_nanny {
class App : public common::App {

    private:
        std::unique_ptr<PubSubClient> _mqtt_client;
   public:

    App();
    ~App() noexcept override = default;
    App(App const&) noexcept = delete;
    App(App&&) noexcept = delete;
    App& operator=(App const&) noexcept = delete;
    App& operator=(App&&) noexcept = delete;

    void initialize() override;
    void run() const override;
    void shutdown() override;
    
    // Network management
    void configure_wifi(const std::string& ssid, const std::string& password);
    bool is_network_connected() const;
    
    // OTA management
    common::patterns::Result<void> perform_ota_update(const std::string& firmware_url);
};
}  // namespace plant_nanny