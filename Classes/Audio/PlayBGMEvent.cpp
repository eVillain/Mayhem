#include "PlayBGMEvent.h"

constexpr PlayBGMEvent::DescriptorType PlayBGMEvent::descriptor;

PlayBGMEvent::PlayBGMEvent(const SongType& type)
: m_type(type)
{
}


