#pragma once

namespace common::utils
{
    class Event
    {
    private:
        /* data */
    public:
        Event(/* args */) = default;
        virtual ~Event() = default;

        virtual void exec() const = 0;
    };
}