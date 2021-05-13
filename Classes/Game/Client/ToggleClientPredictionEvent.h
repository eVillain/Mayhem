#ifndef ToggleClientPredictionEvent_h
#define ToggleClientPredictionEvent_h

#include "Core/Event.h"

class ToggleClientPredictionEvent : public Event
{
public:
    ToggleClientPredictionEvent();
    
    virtual DescriptorType type() const { return descriptor; }

    static constexpr DescriptorType descriptor = "ToggleClientPrediction";    
};

#endif /* ToggleClientPredictionEvent_h */
