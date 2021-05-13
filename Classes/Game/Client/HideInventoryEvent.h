#ifndef HideInventoryEvent_h
#define HideInventoryEvent_h

#include "Core/Event.h"

class HideInventoryEvent : public Event
{
public:
    HideInventoryEvent() {};
    
    virtual DescriptorType type() const { return descriptor; }

    static constexpr DescriptorType descriptor = "HideInventory";
};

constexpr HideInventoryEvent::DescriptorType HideInventoryEvent::descriptor;

#endif /* HideInventoryEvent_h */
