#ifndef Pseudo3DParticle_h
#define Pseudo3DParticle_h

#include "Pseudo3DItem.h"

class Pseudo3DParticle : public Pseudo3DItem
{
public:
    Pseudo3DParticle(const cocos2d::Color4F color,
                     const cocos2d::Vec2 position,
                     const float pseudoZPosition,
                     const cocos2d::Vec2 velocity,
                     const float pseudoZVelocity,
                     const float lifeTime,
                     const float elasticity)
    : Pseudo3DItem(position, pseudoZPosition, velocity, pseudoZVelocity, lifeTime, elasticity)
    , m_color(color) {};

    cocos2d::Color4F getColor() const { return m_color; }
    
//    void update(const float deltaTime) override;

private:
    cocos2d::Color4F m_color;
};

#endif /* Pseudo3DParticle_h */
