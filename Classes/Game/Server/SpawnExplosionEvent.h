#ifndef SpawnExplosionEvent_h
#define SpawnExplosionEvent_h

#include "Core/Event.h"
#include "cocos2d.h"
#include <stdint.h>

class SpawnExplosionEvent : public Event
{
public:
    SpawnExplosionEvent(const uint16_t entityID,
                        const cocos2d::Vec2& position,
                        const float radius,
                        const float force);
    
    virtual DescriptorType type() const { return descriptor; }

    static constexpr DescriptorType descriptor = "SpawnExplosion";
    
    const uint16_t getEntityID() const { return m_entityID; }
    const cocos2d::Vec2 getPosition() const { return m_position; }
    const float getRadius() const { return m_radius; }
    const float getForce() const { return m_force; }

private:
    const uint16_t m_entityID;
    const cocos2d::Vec2 m_position;
    const float m_radius;
    const float m_force;
};

#endif /* SpawnExplosionEvent_h */
