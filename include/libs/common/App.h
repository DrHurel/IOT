#pragma once
#include <any>
#include "libs/common/utils/Event.h"
#include "libs/common/patterns/Singleton.h"

/*
 * Author: hurel
 * Creation date: 2025-09-19
 */

namespace common
{
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
        virtual void run() const = 0;
        virtual void shutdown() = 0;

        virtual void on(std::string_view const &name, utils::Event const &event) = 0;
        virtual void emit(const std::string_view &event) const = 0;
    };
} // namespace common