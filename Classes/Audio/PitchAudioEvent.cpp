#include "PitchAudioEvent.h"

constexpr PitchAudioEvent::DescriptorType PitchAudioEvent::descriptor;

PitchAudioEvent::PitchAudioEvent(const float newPitch, const float duration)
: m_newPitch(newPitch)
, m_duration(duration)
{
}


