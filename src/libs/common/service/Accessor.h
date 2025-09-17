
#pragma once

#include <common/service/Registry.h>

namespace common::service
{

    template <typename ServiceType>
    class Accessor final
    {

    private:
        ServiceType *service_{nullptr};

    public:
        Accessor() noexcept : service_{common::service::DefaultRegistry::instance().get<ServiceType>()} {}

        explicit Accessor(common::service::Registry &registry) noexcept : service_{registry.get<ServiceType>()} {}

        ~Accessor() noexcept = default;
        Accessor(Accessor const &) noexcept = delete;
        Accessor(Accessor &&) noexcept = default;
        Accessor &operator=(Accessor const &) noexcept = delete;
        Accessor &operator=(Accessor &&) noexcept = default;

        bool is_available() const { return service_ != nullptr; }

        ServiceType *operator->() const
        {
            return service_;
        }

        operator ServiceType &() const
        {
            return *service_;
        }

        ServiceType &get() const
        {
            return *service_;
        }
    };

    template <typename ServiceType>
    Accessor<ServiceType> get()
    {
        return Accessor<ServiceType>{};
    }
}