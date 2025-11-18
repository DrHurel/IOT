#pragma once

#include <libs/common/App.h>
#include <memory>
#include <PubSubClient.h>

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
};
}  // namespace plant_nanny