#pragma once
#include <any>
#include <functional>
#include <esp_event.h>
#include "libs/common/patterns/Singleton.h"

/*
 * Author: hurel
 * Creation date: 2025-09-19
 */

namespace common
{
    // Define event base for the application
    ESP_EVENT_DECLARE_BASE(APP_EVENTS);

    using EventHandler = std::function<void(void*, esp_event_base_t, int32_t, void*)>;

    class App : patterns::Singleton<App>
    {
    public:
        App() noexcept = default;
        virtual ~App() noexcept = default;
        App(App const &) noexcept = delete;
        App(App &&) noexcept = delete;
        App &operator=(App const &) noexcept = delete;
        App &operator=(App &&) noexcept = delete;

        virtual void initialize() = 0;
        virtual void run() = 0;
        virtual void shutdown() = 0;

        virtual esp_err_t on(int32_t event_id, esp_event_handler_t handler, void* handler_arg = nullptr) = 0;
        virtual esp_err_t emit(int32_t event_id, void* event_data = nullptr, size_t event_data_size = 0) const = 0;
    };
} // namespace common