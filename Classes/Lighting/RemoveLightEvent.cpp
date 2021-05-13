#include "RemoveLightEvent.h"

constexpr RemoveLightEvent::DescriptorType RemoveLightEvent::descriptor;

RemoveLightEvent::RemoveLightEvent(const size_t lightID)
: m_lightID(lightID)
{
}


