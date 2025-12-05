#pragma once
#include <mutex>
#include <assert.h>
#include <list>
#include <libs/common/patterns/Singleton.h>
#include <libs/common/service/Context.h>
#include <string_view>

namespace common::service::registry
{
    class ContextSwitcher;
}

namespace common::service::implementation
{

    class RegistryImp
    {
    private:
        using Locker = std::unique_lock<std::recursive_mutex>;
        using Contexts = std::list<Context>;
        Contexts contexts_;
        mutable std::recursive_mutex threading_model_;
        friend common::service::registry::ContextSwitcher;

        void push_context(std::string_view context_name)
        {
            Locker locker{threading_model_};
            contexts_.emplace_front(std::move(context_name));
        }

        void pop_context()
        {
            Locker locker{threading_model_};
            contexts_.pop_front();
        }

        Context &get_current_context()
        {
            assert(!contexts_.empty());
            return contexts_.front();
        }

        Context const &get_current_context() const
        {
            assert(!contexts_.empty());
            return contexts_.front();
        }

    public:
        RegistryImp() { contexts_.emplace_back("default"); };
        ~RegistryImp() noexcept = default;
        RegistryImp(RegistryImp const &) noexcept = delete;
        RegistryImp(RegistryImp &&) noexcept = delete;
        RegistryImp &operator=(RegistryImp const &) noexcept = delete;
        RegistryImp &operator=(RegistryImp &&) noexcept = delete;

        template <typename ServiceType, typename InstanceType>
        InstanceType &add(InstanceType &instance)
        {
            Locker locker{threading_model_};
            return get_current_context().template add<ServiceType>(instance);
        }

        template <typename ServiceType, typename InstanceType, typename... Args>
        InstanceType &add(Args &&...args)
        {
            Locker locker{threading_model_};
            return get_current_context().template add<ServiceType, InstanceType>(std::forward<Args>(args)...);
        }

        template <typename ServiceType>
        void remove()
        {
            Locker locker{threading_model_};
            get_current_context().template remove<ServiceType>();
        }

        template <typename ServiceType>
        [[nodiscard]] ServiceType *get() const
        {
            Locker locker{threading_model_};
            for (auto const &context : contexts_)
            {
                if (auto service{context.template get<ServiceType>()}; service != nullptr)
                {
                    return service;
                }
            }

            return nullptr;
        }
    };

}

namespace common::service
{
    struct Registry : public common::service::implementation::RegistryImp
    {
    };

    class DefaultRegistry final : public common::patterns::Singleton<DefaultRegistry>, public Registry
    {
    private:
        DefaultRegistry() = default;
        using Parent = common::patterns::Singleton<DefaultRegistry>;
        friend Parent;
    };

    template <typename ServiceType, typename InstanceType, typename... Args>
    InstanceType &add(Args &&...args)
    {
        return DefaultRegistry::instance().template add<ServiceType, InstanceType>(std::forward<Args>(args)...);
    };

    template <typename ServiceType, typename... Args>
    ServiceType &add(Args &&...args)
    {
        return DefaultRegistry::instance().template add<ServiceType, ServiceType>(std::forward<Args>(args)...);
    };

    template <typename ServiceType, typename InstanceType>
    ServiceType &add(InstanceType &instance)
    {
        DefaultRegistry::instance().template add<ServiceType>(instance);
        return instance;
    };

    template <typename ServiceType>
    void remove()
    {
        DefaultRegistry::instance().template remove<ServiceType>();
    };

    template <typename ServiceType>
    ServiceType exists()
    {
        return DefaultRegistry::instance().template get<ServiceType>() != nullptr;
    };
}
