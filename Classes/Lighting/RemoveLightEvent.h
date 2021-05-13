#ifndef RemoveLightEvent_h
#define RemoveLightEvent_h

#include "Core/Event.h"
#include "LightConstants.h"

class RemoveLightEvent : public Event
{
public:

    RemoveLightEvent(const size_t lightID);
    
    virtual DescriptorType type() const { return descriptor; }

    static constexpr DescriptorType descriptor = "RemoveLight";
    
    const size_t getLightID() const { return m_lightID; }
    
private:
    const size_t m_lightID;
};

#endif /* RemoveLightEvent_h */
