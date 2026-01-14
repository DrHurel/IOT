#include "libs/plant_nanny/states/StateMachine.h"

namespace plant_nanny
{
    void StateMachine::registerState(std::shared_ptr<IAppState> state)
    {
        if (state)
        {
            _states[state->id()] = state;
        }
    }

    bool StateMachine::transitionTo(const std::string& stateId, AppContext& context)
    {
        auto it = _states.find(stateId);
        if (it == _states.end())
        {
            return false;
        }

        if (_currentState)
        {
            _currentState->onExit(context);
        }

        _currentState = it->second;
        _currentStateId = stateId;
        _currentState->onEnter(context);

        return true;
    }

    void StateMachine::handleButton(AppContext& context, services::button::ButtonEvent event)
    {
        if (!_currentState)
        {
            return;
        }

        std::string nextState = _currentState->handleButton(context, event);
        if (!nextState.empty() && nextState != _currentStateId)
        {
            transitionTo(nextState, context);
        }
    }

    void StateMachine::update(AppContext& context)
    {
        if (_currentState)
        {
            _currentState->update(context);
        }
    }

} // namespace plant_nanny
