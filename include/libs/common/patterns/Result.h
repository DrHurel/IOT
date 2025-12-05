#pragma once

#include <string>
#include <optional>
#include <assert.h>
#include <type_traits>
#include <source_location>

namespace common::patterns
{

    class Error
    {
    private:
        std::string message_;
        std::source_location location_;

    public:
        explicit Error(std::string const &message, std::source_location location = std::source_location::current()) : message_(message), location_(location) {}
        std::string const &message() const { return message_; }

        operator std::string() const
        {
            return "Error: " + message_ + " at " + location_.file_name() + ":" + std::to_string(location_.line());
        }
    };

    class BaseResult
    {

    public:
        virtual ~BaseResult() = default;

        virtual bool succeed() const = 0;
        virtual bool failed() const = 0;
    };

    template <typename T>
    class Result : public BaseResult
    {
    public:
        T value() const
        {
            assert(succeed());
            return value_;
        }

        Error error() const
        {
            assert(failed());
            return error_.value();
        }

        bool succeed() const override { return !error_.has_value(); }
        bool failed() const override { return error_.has_value(); }

        static Result<T> failure(Error const &error)
        {
            return Result<T>(error);
        }
        static Result<T> success(T value)
        {
            return Result<T>(value);
        }

        template <typename... Args>
        static Result<T> success(Args... args)
        {
            return Result<T>(args...);
        }

    private:
        T value_;
        std::optional<Error> error_;
        explicit Result(Error const &error) : error_(error) {}
        explicit Result(T value) : value_(value) {}
        template <typename... Args>
        explicit Result(Args... args) : value_(T(args...)) {}
    };

    // Specialization for reference types
    template <typename T>
    class Result<T &> : public BaseResult
    {
    public:
        T &value() const
        {
            assert(succeed());
            assert(value_ != nullptr);
            return *value_;
        }

        Error error() const
        {
            assert(failed());
            return error_.value();
        }

        bool succeed() const override { return !error_.has_value(); }
        bool failed() const override { return error_.has_value(); }

        static Result<T &> failure(Error const &error)
        {
            return Result<T &>(error);
        }

        static Result<T &> success(T &value)
        {
            return Result<T &>(value);
        }

    private:
        std::remove_reference_t<T> *value_ = nullptr;
        std::optional<Error> error_;

        explicit Result(Error const &error) : error_(error) {}
        explicit Result(T &value) : value_(&value) {}
    };

    template <>
    class Result<void> : public BaseResult
    {
    public:
        Result() = default;
        explicit Result(Error const &error) : error_(error) {}

        Error error() const
        {
            assert(error_.has_value());
            return error_.value();
        }

        bool succeed() const override { return !error_.has_value(); }
        bool failed() const override { return error_.has_value(); }

        static Result<void> failure(Error const &error)
        {
            return Result<void>(error);
        }
        static Result<void> success()
        {
            return Result();
        }

    private:
        std::optional<Error> error_;
    };

} // namespace common
