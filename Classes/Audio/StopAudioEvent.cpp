#include "StopAudioEvent.h"

constexpr StopAudioEvent::DescriptorType StopAudioEvent::descriptor;

StopAudioEvent::StopAudioEvent(const int audio)
: m_audio(audio)
{
}


