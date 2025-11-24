#pragma once

#include "libs/common/patterns/Result.h"

namespace plant_nanny::services::pairing
{
    class Manager
    {
    private:
        /* data */
    public:
        Manager(/* args */) = default;
        ~Manager() = default;
        Manager(const Manager&) = delete;
        Manager& operator=(const Manager&) = delete;
        Manager(Manager&&) = delete;
        Manager& operator=(Manager&&) = delete;

        common::patterns::Result<void> start();

        bool is_paired() const;

    };

}