#include "Explosion.h"
#include "Timer.h"

Explosion::Explosion(const uint16_t entityID,
                     const cocos2d::Vec2& position,
                     const float radius,
                     const float force)
: Entity(entityID, EntityType::ExplosionEntity)
, m_radius(0.1)
, m_force(force)
, m_lifeTime(1.0)
, m_startRadius(0.1)
, m_endRadius(radius)
, m_spawnTime(Timer::Seconds())
{
    setPosition(position);
}
