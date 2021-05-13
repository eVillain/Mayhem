#include "AddLightEvent.h"

constexpr AddLightEvent::DescriptorType AddLightEvent::descriptor;

AddLightEvent::AddLightEvent(const LightData& light)
: m_light(light)
{
}


