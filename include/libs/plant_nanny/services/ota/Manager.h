#pragma once

#include "libs/common/patterns/Result.h"
#include "libs/common/logger/Logger.h"
#include "libs/common/service/Accessor.h"
#include <string>
#include <cstdint>
#include <cstddef>
#include <memory>

namespace plant_nanny::services::ota
{
    class Manager
    {
    private:
        /* data */
        common::service::Accessor<common::logger::Logger> logger_;

    public:
        Manager(/* args */);
        ~Manager();
        Manager(const Manager &) = delete;
        Manager &operator=(const Manager &) = delete;
        Manager(Manager &&) = delete;
        Manager &operator=(Manager &&) = delete;
        
        common::patterns::Result<void> start_update();
        bool is_updating() const;
        common::patterns::Result<void> write_chunk(const uint8_t* data, size_t length);
        common::patterns::Result<size_t> get_bytes_written() const;
        common::patterns::Result<void> finalize_update();
        common::patterns::Result<void> abort_update();
        common::patterns::Result<std::string> get_running_app_version() const;
        common::patterns::Result<std::string> get_running_app_name() const;
        common::patterns::Result<void> reboot();
        common::patterns::Result<void> rollback();
        common::patterns::Result<void> mark_valid();
    };

}