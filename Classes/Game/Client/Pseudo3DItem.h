#ifndef Pseudo3DItem_h
#define Pseudo3DItem_h

#include "cocos2d.h"

class Pseudo3DItem
{
public:
    Pseudo3DItem(const cocos2d::Vec2 position,
                 const float pseudoZPosition,
                 const cocos2d::Vec2 velocity,
                 const float pseudoZVelocity,
                 const float lifeTime,
                 const float elasticity);
    
    virtual void update(const float deltaTime);
    cocos2d::Vec2 getScreenPosition() const;
    
    cocos2d::Vec2 getPosition() const { return m_position; }
    float getPseudoZPosition() const { return m_pseudoZPosition; }
    void setPseudoZPosition(const float position) { m_pseudoZPosition = position; }

    cocos2d::Vec2 getVelocity() const { return m_velocity; }
    float getPseudoZVelocity() const { return m_pseudoZVelocity; }
    
    void setVelocity(const cocos2d::Vec2& velocity) { m_velocity = velocity; }
    void setPseudoZVelocity(const float velocity) { m_pseudoZVelocity = velocity; }

    float getLifeTime() const { return m_lifeTime; }
    float getTimeAlive() const { return m_timeAlive; }
    
protected:
    cocos2d::Vec2 m_position;
    float m_pseudoZPosition;
    
    cocos2d::Vec2 m_velocity;
    float m_pseudoZVelocity;
    
    float m_lifeTime;
    float m_timeAlive;
    
    float m_elasticity;
};

#endif /* Pseudo3DItem_h */
