#ifndef ToggleInventoryEvent_h
#define ToggleInventoryEvent_h

#include "Core/Event.h"

class ToggleInventoryEvent : public Event
{
public:
    ToggleInventoryEvent();
    
    virtual DescriptorType type() const { return descriptor; }

    static constexpr DescriptorType descriptor = "ToggleInventory";    
};

#endif /* ToggleInventoryEvent_h */
