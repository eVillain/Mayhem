#ifndef InputActionEvent_h
#define InputActionEvent_h

class InputActionEvent
{
public:
    InputActionEvent(const std::string& action,
                     const float value,
                     const float previousValue)
    : action(action)
    , value(value)
    , previousValue(previousValue)
    {};
    
    const std::string& action;
    const float value;
    const float previousValue;
};

#endif /* InputActionEvent_h */
