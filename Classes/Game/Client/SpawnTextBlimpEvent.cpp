#include "SpawnTextBlimpEvent.h"

constexpr SpawnTextBlimpEvent::DescriptorType SpawnTextBlimpEvent::descriptor;

SpawnTextBlimpEvent::SpawnTextBlimpEvent(const std::string text, const cocos2d::Vec2 position, const int size, const float duration, const cocos2d::Color4B& color)
: m_text(text)
, m_position(position)
, m_size(size)
, m_duration(duration)
, m_color(color)
{
}
