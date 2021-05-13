#include "Entity.h"

Entity::Entity(const uint16_t entityID, EntityType type)
: m_entityID(entityID)
, m_entityType(type)
, m_position(cocos2d::Vec2::ZERO)
, m_rotation(0.f)
, m_velocity(cocos2d::Vec2::ZERO)
, m_angularVelocity(0.f)
, m_isRemoved(false)
{
}
