#ifndef InputActivityEvent_h
#define InputActivityEvent_h

#include "InputConstants.h"

class InputActivityEvent
{
public:
    InputActivityEvent(const int activeController,
                       const std::string& name,
                       const GamepadType type)
    : activeController(activeController)
    , name(name)
    , type(type)
    {};
    
    const int activeController;
    const std::string& name;
    const GamepadType type;
};

#endif /* InputActivityEvent_h */
