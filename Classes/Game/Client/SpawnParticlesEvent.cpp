#include "SpawnParticlesEvent.h"

constexpr SpawnParticlesEvent::DescriptorType SpawnParticlesEvent::descriptor;

SpawnParticlesEvent::SpawnParticlesEvent(const std::string& type,
                                         const cocos2d::Vec2& position,
                                         const float rotation,
                                         const cocos2d::Vec2& endPos,
                                         const bool selfLit)
: m_type(type)
, m_position(position)
, m_rotation(rotation)
, m_endPosition(endPos)
, m_selfLit(selfLit)
{
}

constexpr SpawnParticlesAttachedEvent::DescriptorType SpawnParticlesAttachedEvent::descriptor;

SpawnParticlesAttachedEvent::SpawnParticlesAttachedEvent(const std::string& type,
                                                         cocos2d::Node* parent,
                                                         const float rotation,
                                                         const cocos2d::Vec2& offset,
                                                         const cocos2d::ParticleSystem::PositionType positionType,
                                                         const bool selfLit)
: m_type(type)
, m_parent(parent)
, m_rotation(rotation)
, m_offset(offset)
, m_positionType(positionType)
, m_selfLit(selfLit)
{
}
