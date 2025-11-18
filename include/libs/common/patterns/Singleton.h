#pragma once

#include <memory>
#include <mutex>
#include <cassert>
#include <ostream>
#include <utility>

#include <atomic>
#include <string_view>

namespace common::patterns
{

    namespace singleton
    {
        size_t number_of_instances();
        void dump_registry(std::ostream &ostream, std::string_view separator);
    }

    template <typename Type>
    class Singleton
    {
    private:
        inline static std::unique_ptr<Type> instance_;
        inline static std::atomic<bool> is_created_{false};

    protected:
        Singleton() noexcept = default;
        ~Singleton() noexcept = default;

    public:
        Singleton(Singleton const &) noexcept = delete;
        Singleton(Singleton &&) noexcept = delete;
        Singleton &operator=(Singleton const &) noexcept = delete;
        Singleton &operator=(Singleton &&) noexcept = delete;
        template <typename... Args>
        static void create(Args &&...args)
        {
            bool expected = false;
            if (is_created_.compare_exchange_strong(expected, true))
            {
                instance_.reset(new Type(std::forward<Args>(args)...));
            }
        }

        static void destroy()
        {
            bool expected = true;
            if (is_created_.compare_exchange_strong(expected, false))
            {
                instance_.reset();
            }
        }

        static Type &instance() noexcept
        {
            assert(instance_ != nullptr);
            return *instance_.get();
        }

        static bool has_been_created() { return is_created_.load(); }
    };

}