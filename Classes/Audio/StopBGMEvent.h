#ifndef StopBGMEvent_h
#define StopBGMEvent_h

#include "Core/Event.h"

class StopBGMEvent : public Event
{
public:

    StopBGMEvent();
    
    virtual DescriptorType type() const { return descriptor; }

    static constexpr DescriptorType descriptor = "StopBGM";
};

#endif /* StopBGMEvent_h */
