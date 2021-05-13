#ifndef AddLightEvent_h
#define AddLightEvent_h

#include "Core/Event.h"
#include "LightConstants.h"

class AddLightEvent : public Event
{
public:
    AddLightEvent(const LightData& light);
    
    virtual DescriptorType type() const { return descriptor; }

    static constexpr DescriptorType descriptor = "AddLight";
    
    const LightData getLight() const { return m_light; }

private:
    LightData m_light;
};

#endif /* AddLightEvent_h */
