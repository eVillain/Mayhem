#ifndef SpawnParticlesEvent_h
#define SpawnParticlesEvent_h

#include "ParticleConstants.h"
#include "cocos2d.h"

class SpawnParticlesEvent
{
public:
    SpawnParticlesEvent(const std::string& type,
                        const cocos2d::Vec2& position,
                        const float rotation = 0,
                        const cocos2d::Vec2& endPos = cocos2d::Vec2::ZERO,
                        const bool selfLit = false)
    : m_type(type)
    , m_position(position)
    , m_rotation(rotation)
    , m_endPosition(endPos)
    , m_selfLit(selfLit)
    {}

    const std::string& getParticlesType() const { return m_type; }
    const cocos2d::Vec2& getPosition() const { return m_position; }
    const float getRotation() const { return m_rotation; }
    const cocos2d::Vec2& getEndPosition() const { return m_endPosition; }
    const bool getSelfLit() const { return m_selfLit; }

private:
    const std::string& m_type;
    const cocos2d::Vec2 m_position;
    const float m_rotation;
    const cocos2d::Vec2 m_endPosition;
    bool m_selfLit;
};

class SpawnParticlesAttachedEvent
{
public:
    SpawnParticlesAttachedEvent(const std::string& type,
                                cocos2d::Node* parent,
                                const float rotation = 0,
                                const cocos2d::Vec2& offset=cocos2d::Vec2::ZERO,
                                const cocos2d::ParticleSystem::PositionType positionType = cocos2d::ParticleSystem::PositionType::GROUPED,
                                const bool selfLit = false)
    : m_type(type)
    , m_parent(parent)
    , m_rotation(rotation)
    , m_offset(offset)
    , m_positionType(positionType)
    , m_selfLit(selfLit)
    {}
    
    const std::string& getParticlesType() const { return m_type; }
    cocos2d::Node* getParent() const { return m_parent; }
    const float getRotation() const { return m_rotation; }
    const cocos2d::Vec2 getOffset() const { return m_offset; }
    const cocos2d::ParticleSystem::PositionType getPositionType() const { return m_positionType; }
    const bool getSelfLit() const { return m_selfLit; }

private:
    const std::string& m_type;
    cocos2d::Node* m_parent;
    const float m_rotation;
    const cocos2d::Vec2 m_offset;
    cocos2d::ParticleSystem::PositionType m_positionType;
    bool m_selfLit;
};

#endif /* SpawnParticlesEvent_h */
