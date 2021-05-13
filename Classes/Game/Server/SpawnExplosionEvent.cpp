#include "SpawnExplosionEvent.h"

constexpr SpawnExplosionEvent::DescriptorType SpawnExplosionEvent::descriptor;

SpawnExplosionEvent::SpawnExplosionEvent(const uint16_t entityID,
                                         const cocos2d::Vec2& position,
                                         const float radius,
                                         const float force)
: m_entityID(entityID)
, m_position(position)
, m_radius(radius)
, m_force(force)
{
}
