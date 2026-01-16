#pragma once

#include <libs/common/service/Service.h>

#include <unordered_map>
#include <type_traits>
#include <string>
#include <string_view>

namespace common::service
{

    class Context final
    {
    private:
        using Services = std::unordered_map<std::size_t, std::unique_ptr<Service>>;
        Services services_;
        std::string name_;
        template <typename ThreadingModel>
        friend class RegistryImp;

    public:
        explicit Context(std::string_view name) noexcept;
        ~Context() noexcept = default;
        Context(Context const &) noexcept = delete;
        Context(Context &&) noexcept = default;
        Context &operator=(Context const &) noexcept = delete;
        Context &operator=(Context &&) noexcept = delete;

        template <typename ServiceType, typename InstanceType>
        InstanceType &add(InstanceType &service)
        {
            static_assert(std::is_base_of_v<ServiceType, InstanceType>);
            static auto const class_id{typeid(ServiceType).hash_code()};
            auto new_service = std::make_unique<ServiceInstance<ServiceType>>(&service);
            auto service_ptr = new_service->get_instance();
            services_[class_id] = std::move(new_service);
            return *dynamic_cast<InstanceType *>(service_ptr);
        }

        template <typename ServiceType, typename InstanceType, typename... Args>
        InstanceType &add(Args &&...args)
        {
            static_assert(std::is_base_of_v<ServiceType, InstanceType>);
            static auto const class_id{typeid(ServiceType).hash_code()};
            auto new_service = std::make_unique<ServiceInstance<ServiceType>>(std::make_unique<InstanceType>(std::forward<Args>(args)...));
            auto service_ptr = new_service->get_instance();
            services_[class_id] = std::move(new_service);
            return *dynamic_cast<InstanceType *>(service_ptr);
        }

        template <typename ServiceType>
        void remove()
        {
            static auto const class_id{typeid(ServiceType).hash_code()};
            services_.erase(class_id);
        }

        template <typename ServiceType>
        ServiceType *get() const
        {
            static auto const class_id{typeid(ServiceType).hash_code()};
            auto service_itr = services_.find(class_id);
            if (service_itr == std::end(services_))
            {
                return nullptr;
            }

            auto service{service_itr->second.get()};
            auto instance{dynamic_cast<ServiceInstance<ServiceType> *>(service)};

            return instance->get_instance();
        }
    };

}