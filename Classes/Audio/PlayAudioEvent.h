#ifndef PlayAudioEvent_h
#define PlayAudioEvent_h

#include "Core/Event.h"
#include "cocos2d.h"
#include <string>

class PlayAudioEvent : public Event
{
public:
    PlayAudioEvent(const std::string& audioFile, const float volume = 1.0f);
    
    virtual DescriptorType type() const { return descriptor; }

    static constexpr DescriptorType descriptor = "PlayAudio";
    
    const std::string& getAudioFile() const { return m_audioFile; }
    const float getVolume() const { return m_volume; }
private:
    std::string m_audioFile;
    const float m_volume;
};

class PlayPositionalAudioEvent : public PlayAudioEvent
{
public:
    PlayPositionalAudioEvent(const std::string& audioFile, const cocos2d::Vec2 position, const float volume = 1.0f);
    
    virtual DescriptorType type() const { return descriptor; }

    static constexpr DescriptorType descriptor = "PlayPositionalAudio";
    
    const cocos2d::Vec2& getPosition() const { return m_position; }
private:
    cocos2d::Vec2 m_position;
};

#endif /* PlayAudioEvent_h */
