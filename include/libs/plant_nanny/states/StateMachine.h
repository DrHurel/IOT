#pragma once

#include "IAppState.h"
#include "AppContext.h"
#include <unordered_map>
#include <memory>
#include <string>

namespace plant_nanny
{
    /**
     * @brief State machine manager (SRP - only manages state transitions)
     */
    class StateMachine
    {
    private:
        std::unordered_map<std::string, std::shared_ptr<IAppState>> _states;
        std::shared_ptr<IAppState> _currentState;
        std::string _currentStateId;

    public:
        StateMachine() = default;
        ~StateMachine() = default;

        StateMachine(const StateMachine&) = delete;
        StateMachine& operator=(const StateMachine&) = delete;
        StateMachine(StateMachine&&) = default;
        StateMachine& operator=(StateMachine&&) = default;

        /**
         * @brief Register a state
         */
        void registerState(std::shared_ptr<IAppState> state);

        /**
         * @brief Transition to a new state
         * @return true if transition successful
         */
        bool transitionTo(const std::string& stateId, AppContext& context);

        /**
         * @brief Handle button event in current state
         */
        void handleButton(AppContext& context, services::button::ButtonEvent event);

        /**
         * @brief Update current state
         */
        void update(AppContext& context);

        /**
         * @brief Get current state ID
         */
        const std::string& currentStateId() const { return _currentStateId; }
    };

} // namespace plant_nanny
