#ifndef TogglePhysicsDebugEvent_h
#define TogglePhysicsDebugEvent_h

#include "Core/Event.h"

class TogglePhysicsDebugEvent : public Event
{
public:
    TogglePhysicsDebugEvent();
    
    virtual DescriptorType type() const { return descriptor; }

    static constexpr DescriptorType descriptor = "TogglePhysics";
};

#endif /* TogglePhysicsDebugEvent_h */
