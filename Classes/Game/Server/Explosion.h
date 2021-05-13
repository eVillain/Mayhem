#ifndef Explosion_h
#define Explosion_h

#include "Entity.h"

class Explosion : public Entity
{
public:
    Explosion(const uint16_t entityID,
              const cocos2d::Vec2& position,
              const float radius,
              const float force);

private:
    float m_lifeTime;
    float m_radius;
    float m_force;

    float m_startRadius;
    float m_endRadius;
    
    int m_spawnTime;
};

#endif /* Explosion_h */
