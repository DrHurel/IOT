#pragma once

#include "libs/common/patterns/Result.h"

namespace plant_nanny::services::network
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

        common::patterns::Result<void> connect();
        bool is_connected() const;

    };

}