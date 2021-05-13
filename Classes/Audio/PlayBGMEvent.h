#ifndef PlayBGMEvent_h
#define PlayBGMEvent_h

#include "Core/Event.h"

class PlayBGMEvent : public Event
{
public:
    enum SongType
    {
        SONG_1,
    };
    PlayBGMEvent(const SongType &type);
    
    virtual DescriptorType type() const { return descriptor; }

    static constexpr DescriptorType descriptor = "PlayBGM";
    
    const SongType getBGMType() const { return m_type; }
private:
    SongType m_type;
    
};

#endif /* PlayBGMEvent_h */
