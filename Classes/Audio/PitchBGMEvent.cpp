#include "PitchBGMEvent.h"

constexpr PitchBGMEvent::DescriptorType PitchBGMEvent::descriptor;

PitchBGMEvent::PitchBGMEvent(const float newPitch)
: m_newPitch(newPitch)
{
}


