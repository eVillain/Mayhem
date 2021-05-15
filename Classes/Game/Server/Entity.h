#ifndef Entity_h
#define Entity_h

#include "cocos2d.h"
#include "Game/Shared/EntityConstants.h"

class Entity
{
public:
    Entity(const uint16_t entityID,
           const EntityType type);
    
    const uint16_t getEntityID() const { return m_entityID; }
    const EntityType& getEntityType() const { return m_entityType; }
    
    virtual const cocos2d::Vec2& getPosition() const { return m_position; }
    virtual const float getRotation() const { return m_rotation; }
    virtual const cocos2d::Vec2& getVelocity() const { return m_velocity; }
    virtual const float getAngularVelocity() const { return m_angularVelocity; }
    virtual const bool getIsRemoved() const { return m_isRemoved; }

    virtual void setPosition(const cocos2d::Vec2& position) { m_position = position; }
    virtual void setRotation(const float rotation) { m_rotation = rotation; }
    virtual void setVelocity(const cocos2d::Vec2& velocity) { m_velocity = velocity; }
    virtual void setAngularVelocity(const float angularVelocity) { m_angularVelocity = angularVelocity; }
    virtual void setIsRemoved(const bool isRemoved) { m_isRemoved = isRemoved; }
    
protected:
    const uint16_t m_entityID;
    EntityType m_entityType;
    cocos2d::Vec2 m_position;
    float m_rotation;
    cocos2d::Vec2 m_velocity;
    float m_angularVelocity;
    bool m_isRemoved;
};

#endif /* Entity_h */
