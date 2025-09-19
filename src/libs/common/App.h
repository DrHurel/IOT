#pragma once

/*
 * Author: hurel
 * Creation date: 2025-09-19
 */

namespace um {
class App {
   public:
    App() noexcept = default;
    virtual ~App() noexcept = default;
    App(App const&) noexcept = delete;
    App(App&&) noexcept = delete;
    App& operator=(App const&) noexcept = delete;
    App& operator=(App&&) noexcept = delete;

    virtual void initialize() = 0;
    virtual void run() const = 0;
    virtual void shutdown() = 0;
};
} // namespace um
