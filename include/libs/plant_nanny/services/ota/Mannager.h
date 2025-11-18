#pragma once

#include "libs/common/patterns/Result.h"

namespace plant_nanny::services::ota
{
    class Mannager
    {
    private:
        /* data */
    public:
        Mannager(/* args */) = default;
        ~Mannager() = default;
        Mannager(const Mannager &) = delete;
        Mannager &operator=(const Mannager &) = delete;
        Mannager(Mannager &&) = delete;
        Mannager &operator=(Mannager &&) = delete;

        common::patterns::Result<void> start_update();
        bool is_updating() const;
        common::patterns::Result<void> finalize_update();
    };

}