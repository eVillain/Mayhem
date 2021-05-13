#ifndef PitchBGMEvent_h
#define PitchBGMEvent_h

#include "Core/Event.h"

class PitchBGMEvent : public Event
{
public:

    PitchBGMEvent(const float newPitch);
    
    virtual DescriptorType type() const { return descriptor; }

    static constexpr DescriptorType descriptor = "PitchBGM";
    
    const float getNewPitch() const { return m_newPitch; }
private:
    const float m_newPitch;
};

#endif /* PitchBGMEvent_h */
