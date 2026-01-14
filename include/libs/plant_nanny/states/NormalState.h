#pragma once

#include "libs/plant_nanny/states/IAppState.h"
#include "libs/plant_nanny/states/AppContext.h"
#include "libs/common/service/Accessor.h"
#include "libs/common/logger/Logger.h"

namespace plant_nanny::states
{
    namespace
    {
        inline void log_state(const char* message)
        {
            auto logger = common::service::get<common::logger::Logger>();
            if (logger.is_available())
            {
                logger->info(message);
            }
        }
    }

    /**
     * @brief Normal/idle state - waiting for user input
     */
    class NormalState : public IAppState
    {
    public:
        static constexpr const char* ID = "normal";

        const char* id() const override { return ID; }

        void onEnter(AppContext& context) override
        {
            context.screenManager().navigateTo("normal");
            log_state("[STATE] Entered Normal");
        }

        void onExit(AppContext& context) override {}

        std::string handleButton(AppContext& context, services::button::ButtonEvent event) override
        {
            using namespace services::button;

            switch (event)
            {
                case ButtonEvent::LEFT_LONG_PRESS:
                    log_state("[APP] BT pairing...");
                    return "pairing";

                case ButtonEvent::RIGHT_LONG_PRESS:
                    log_state("[APP] Reset...");
                    return "resetting";

                default:
                    return "";
            }
        }

        void update(AppContext& context) override {}
    };

} // namespace plant_nanny::states
