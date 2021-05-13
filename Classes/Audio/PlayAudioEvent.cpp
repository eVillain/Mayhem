#include "PlayAudioEvent.h"

constexpr PlayAudioEvent::DescriptorType PlayAudioEvent::descriptor;

PlayAudioEvent::PlayAudioEvent(const std::string& audioFile,
                               const float volume)
: m_audioFile(audioFile)
, m_volume(volume)
{
}

constexpr PlayPositionalAudioEvent::DescriptorType PlayPositionalAudioEvent::descriptor;

PlayPositionalAudioEvent::PlayPositionalAudioEvent(const std::string& audioFile,
                                                   const cocos2d::Vec2 position,
                                                   const float volume)
: PlayAudioEvent(audioFile, volume)
, m_position(position)
{
}
