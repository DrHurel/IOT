
#pragma once

#include <libs/common/service/Registry.h>

namespace common::service
{

    template <typename ServiceType>
    class Accessor final
    {

    private:
        ServiceType *service_{nullptr};
        bool initialized_{false};
        
        void lazy_init() {
            if (!initialized_ && common::service::DefaultRegistry::has_been_created()) {
                service_ = common::service::DefaultRegistry::instance().get<ServiceType>();
                initialized_ = true;
            }
        }

    public:
        Accessor() noexcept = default;

        explicit Accessor(common::service::Registry &registry) noexcept : service_{registry.get<ServiceType>()}, initialized_{true} {}

        ~Accessor() noexcept = default;
        Accessor(Accessor const &) noexcept = delete;
        Accessor(Accessor &&) noexcept = default;
        Accessor &operator=(Accessor const &) noexcept = delete;
        Accessor &operator=(Accessor &&) noexcept = default;

        bool is_available() { 
            lazy_init();
            return service_ != nullptr; 
        }

        ServiceType *operator->()
        {
            lazy_init();
            return service_;
        }

        operator ServiceType &()
        {
            lazy_init();
            return *service_;
        }

        ServiceType &get()
        {
            lazy_init();
            return *service_;
        }
    };

    template <typename ServiceType>
    Accessor<ServiceType> get()
    {
        return Accessor<ServiceType>{};
    }
}