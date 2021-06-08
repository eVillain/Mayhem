#ifndef InputConnectedEvent_h
#define InputConnectedEvent_h

#include "InputConstants.h"

class InputConnectedEvent
{
public:
    InputConnectedEvent(const int jid,
                       const std::string& name,
                       const GamepadType type,
                       bool isConnected)
    : jid(jid)
    , name(name)
    , type(type)
    , isConnected(isConnected)
    {};
    
    const int jid;
    const std::string& name;
    const GamepadType type;
    const bool isConnected;
};

#endif /* InputConnectedEvent_h */
