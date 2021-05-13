#include "SpawnProjectileEvent.h"

constexpr SpawnProjectileEvent::DescriptorType SpawnProjectileEvent::descriptor;

SpawnProjectileEvent::SpawnProjectileEvent(const uint16_t entityID,
                                           const ProjectileType type,
                                           const cocos2d::Vec2& position,
                                           const cocos2d::Vec2& velocity,
                                           const float rotation)
: m_entityID(entityID)
, m_type(type)
, m_position(position)
, m_velocity(velocity)
, m_rotation(rotation)
{
}
