#pragma once
#include <memory>

namespace common::service
{
    class Service
    {

    public:
        Service() noexcept = default;
        virtual ~Service() noexcept = default;
        Service(Service const &) noexcept = delete;
        Service(Service &&) noexcept = delete;
        Service &operator=(Service const &) noexcept = delete;
        Service &operator=(Service &&) noexcept = delete;
    };

    template <typename ServiceType>
    class ServiceInstance final : public Service
    {
    private:
        std::unique_ptr<ServiceType> instance_;
        bool owned_{false};

    public:
        explicit ServiceInstance(std::unique_ptr<ServiceType> instance) noexcept
            : instance_{std::move(instance)}, owned_{true} {}

        explicit ServiceInstance(ServiceType *instance) noexcept
            : instance_{instance}, owned_{false} {}

        ~ServiceInstance() override
        {
            if (!owned_)
            {
                instance_.release();
            }
        }

        ServiceType *get_instance() const { return instance_.get(); }
    };
}