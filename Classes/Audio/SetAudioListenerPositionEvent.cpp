#include "SetAudioListenerPositionEvent.h"

constexpr SetAudioListenerPositionEvent::DescriptorType SetAudioListenerPositionEvent::descriptor;

SetAudioListenerPositionEvent::SetAudioListenerPositionEvent(const cocos2d::Vec2& position)
: m_position(position)
{
}
