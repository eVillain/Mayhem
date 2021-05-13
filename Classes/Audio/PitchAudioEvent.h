#ifndef PitchAudioEvent_h
#define PitchAudioEvent_h

#include "Core/Event.h"

class PitchAudioEvent : public Event
{
public:

    PitchAudioEvent(const float newPitch, const float duration);
    
    virtual DescriptorType type() const { return descriptor; }

    static constexpr DescriptorType descriptor = "PitchAudio";
    
    const float getNewPitch() const { return m_newPitch; }
    const float getDuration() const { return m_duration; }
private:
    const float m_newPitch;
    const float m_duration;
};

#endif /* PitchAudioEvent_h */
