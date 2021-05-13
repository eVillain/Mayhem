#ifndef ToggleMuteBGMEvent_h
#define ToggleMuteBGMEvent_h

#include "Core/Event.h"

class ToggleMuteBGMEvent : public Event
{
public:

    ToggleMuteBGMEvent();
    
    virtual DescriptorType type() const { return descriptor; }

    static constexpr DescriptorType descriptor = "ToggleMuteBGM";
};

#endif /* ToggleMuteBGMEvent_h */
