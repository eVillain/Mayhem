#ifndef SpawnProjectileEvent_h
#define SpawnProjectileEvent_h

#include "Core/Event.h"
#include "Projectile.h"

typedef enum
{
    FragNade,
    SmokeNade,
    Stone,
    Bullet,
    Rocket
} ProjectileType;

class SpawnProjectileEvent : public Event
{
public:
    SpawnProjectileEvent(const uint16_t entityID,
                         const ProjectileType type,
                         const cocos2d::Vec2& position,
                         const cocos2d::Vec2& velocity,
                         const float rotation = 0.0f);
    
    virtual DescriptorType type() const { return descriptor; }

    static constexpr DescriptorType descriptor = "SpawnProjectile";
    
    const uint16_t getEntityID() const { return m_entityID; }
    const ProjectileType getProjectileType() const { return m_type; }
    const cocos2d::Vec2 getPosition() const { return m_position; }
    const cocos2d::Vec2 getVelocity() const { return m_velocity; }
    const float getRotation() const { return m_rotation; }

private:
    const uint16_t m_entityID;
    const ProjectileType m_type;
    const cocos2d::Vec2 m_position;
    const cocos2d::Vec2 m_velocity;
    const float m_rotation;
};

#endif /* SpawnProjectileEvent_h */
