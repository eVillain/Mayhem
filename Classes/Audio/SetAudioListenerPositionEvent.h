#ifndef SetAudioListenerPositionEvent_h
#define SetAudioListenerPositionEvent_h

#include "Core/Event.h"
#include "cocos2d.h"

class SetAudioListenerPositionEvent : public Event
{
public:
    SetAudioListenerPositionEvent(const cocos2d::Vec2& position);
    
    virtual DescriptorType type() const { return descriptor; }

    static constexpr DescriptorType descriptor = "SetAudioListenerPosition";
    
    const cocos2d::Vec2& getPosition() const { return m_position; }
    
private:
    cocos2d::Vec2 m_position;
};

#endif /* SetAudioListenerPositionEvent_h */
