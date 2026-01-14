#pragma once
namespace plant_nanny::services::pump_manager
{

    class PumpManager
    {
    public:
        PumpManager() = default;
        ~PumpManager() = default;
        PumpManager(const PumpManager &) = delete;
        PumpManager(PumpManager &&) = delete;
        PumpManager &operator=(const PumpManager &) = delete;
        PumpManager &operator=(PumpManager &&) = delete;
    };

}