#include "StateMachine.h"

StateMachine::StateMachine(const int initialState)
: m_currentState(initialState)
{
}

void StateMachine::addTransition(const int state, const std::vector<Transition>& t)
{
    auto result = m_transitions.find(state);
    if (result != m_transitions.end())
    {
        result->second.insert(result->second.end(), t.begin(), t.end());
    }
    else
    {
        m_transitions[state] = std::move(t);
    }
}

void StateMachine::addUpdate(const int state, StateUpdate &u)
{
    m_updates[state] = std::move(u);
}

void StateMachine::update()
{
    m_stateChanged = false;
    auto transitionResult = m_transitions.find(m_currentState);
    if (transitionResult != m_transitions.end())
    {
        for (auto transition : transitionResult->second)
        {
            if (transition.criteria())
            {
                m_currentState = transition.newState;
                m_stateChanged = true;
                break;
            }
        }
    }

    auto updateResult = m_updates.find(m_currentState);
    if (updateResult != m_updates.end())
    {
        updateResult->second(m_stateChanged);
    }
}