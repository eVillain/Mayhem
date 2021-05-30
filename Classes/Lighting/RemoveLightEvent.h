#ifndef RemoveLightEvent_h
#define RemoveLightEvent_h

#include "LightConstants.h"

class RemoveLightEvent
{
public:
    RemoveLightEvent(const size_t lightID)
    : m_lightID(lightID)
    {}

    const size_t getLightID() const { return m_lightID; }

private:
    const size_t m_lightID;
};

#endif /* RemoveLightEvent_h */
