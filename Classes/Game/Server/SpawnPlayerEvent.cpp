#include "SpawnPlayerEvent.h"

constexpr SpawnPlayerEvent::DescriptorType SpawnPlayerEvent::descriptor;

SpawnPlayerEvent::SpawnPlayerEvent(const uint16_t entityID,
                                   const uint8_t playerID,
                                   const cocos2d::Vec2& position,
                                   const float rotation)
: m_entityID(entityID)
, m_playerID(playerID)
, m_position(position)
, m_rotation(rotation)
{
}
