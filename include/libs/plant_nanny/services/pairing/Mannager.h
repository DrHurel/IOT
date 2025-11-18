#pragma once

#include "libs/common/patterns/Result.h"

namespace plant_nanny::services::pairing
{
    class Mannager
    {
    private:
        /* data */
    public:
        Mannager(/* args */) = default;
        ~Mannager() = default;
        Mannager(const Mannager&) = delete;
        Mannager& operator=(const Mannager&) = delete;
        Mannager(Mannager&&) = delete;
        Mannager& operator=(Mannager&&) = delete;

        common::patterns::Result<void> start();

        bool is_paired() const;

    };

}