#pragma once

namespace common::utils
{
    class Event
    {
    private:
        /* data */
    public:
        Event(/* args */);
        ~Event();

        virtual void exec();
    };
}