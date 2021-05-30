#ifndef SpawnTextBlimpEvent_h
#define SpawnTextBlimpEvent_h

#include "cocos2d.h"
#include <string>

class SpawnTextBlimpEvent
{
public:
    SpawnTextBlimpEvent(const std::string text,
                        const cocos2d::Vec2 position,
                        const int size,
                        const float duration,
                        const cocos2d::Color4B& color)
    : m_text(text)
    , m_position(position)
    , m_size(size)
    , m_duration(duration)
    , m_color(color)
    {}

    const std::string getText() const { return m_text; }
    const cocos2d::Vec2 getPosition() const { return m_position; }
    const int getSize() const { return m_size; }
    const float getDuration() const { return m_duration; }
    const cocos2d::Color4B getColor() const { return m_color; }
private:
    const std::string m_text;
    const cocos2d::Vec2 m_position;
    const int m_size;
    const float m_duration;
    const cocos2d::Color4B m_color;
};

#endif /* SpawnTextBlimpEvent_h */
