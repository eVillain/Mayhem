#ifndef AddLightEvent_h
#define AddLightEvent_h

#include "LightConstants.h"

class AddLightEvent
{
public:
    AddLightEvent(const LightData& light)
    : m_light(light)
    {}

    const LightData getLight() const { return m_light; }

private:
    LightData m_light;
};

#endif /* AddLightEvent_h */
