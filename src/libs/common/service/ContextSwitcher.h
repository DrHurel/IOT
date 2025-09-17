#pragma once

#include <string_view>

namespace common::service
{
    class Registry;
}

namespace common::service::registry
{

    class ContextSwitcher
    {
    private:
        common::service::Registry &registry_;

    public:
        using Registry = common::service::Registry;
        ContextSwitcher(std::string_view context_name, Registry &registry);
        ~ContextSwitcher();
        ContextSwitcher(ContextSwitcher const &) noexcept = delete;
        ContextSwitcher(ContextSwitcher &&) noexcept = delete;
        ContextSwitcher &operator=(ContextSwitcher const &) noexcept = delete;
        ContextSwitcher &operator=(ContextSwitcher &&) noexcept = delete;
    };

}