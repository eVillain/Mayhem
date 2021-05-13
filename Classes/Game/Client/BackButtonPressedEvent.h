#ifndef BackButtonPressedEvent_h
#define BackButtonPressedEvent_h

#include "Core/Event.h"

class BackButtonPressedEvent : public Event
{
public:
    BackButtonPressedEvent() {};
    
    virtual DescriptorType type() const { return descriptor; }

    static constexpr DescriptorType descriptor = "BackButtonPressed";
};

#endif /* BackButtonPressedEvent_h */
