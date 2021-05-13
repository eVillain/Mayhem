#ifndef StopAudioEvent_h
#define StopAudioEvent_h

#include "Core/Event.h"

class StopAudioEvent : public Event
{
public:

    StopAudioEvent(const int audio);
    
    virtual DescriptorType type() const { return descriptor; }

    static constexpr DescriptorType descriptor = "StopAudio";
    
    const int getAudio() const { return m_audio; }
    
private:
    const int m_audio;
};

#endif /* StopAudioEvent_h */
