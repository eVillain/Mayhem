#ifndef StateMachine_h
#define StateMachine_h

#include <functional>
#include <map>
#include <vector>

typedef std::function<bool()> TransitionCondition;
typedef std::function<void(bool)> StateUpdate;

struct Transition
{
    TransitionCondition criteria;
    int newState;
    
    Transition(TransitionCondition c, const int n)
    : criteria(c), newState(n) {}
};

class StateMachine
{
public:
    StateMachine(const int initialState);
    
    void addTransition(const int state, const std::vector<Transition>& t);
    void addUpdate(const int state, StateUpdate& u);
    
    void update();
    
    const int getCurrentState() const { return m_currentState; }
    const bool getStateChanged() const { return m_stateChanged; }
private:
    std::map<int, std::vector<Transition>> m_transitions;
    std::map<int, StateUpdate> m_updates;
    int m_currentState;
    bool m_stateChanged;
};

#endif /* StateMachine_h */
