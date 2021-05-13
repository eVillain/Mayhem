#ifndef Pseudo3DSprite_h
#define Pseudo3DSprite_h

#include "Pseudo3DItem.h"

class Pseudo3DSprite : public Pseudo3DItem
{
public:
    Pseudo3DSprite(cocos2d::Sprite* sprite,
                   const cocos2d::Vec2 position,
                   const float pseudoZPosition,
                   const cocos2d::Vec2 velocity,
                   const float pseudoZVelocity,
                   const float lifeTime,
                   const float elasticity)
    : Pseudo3DItem(position, pseudoZPosition, velocity, pseudoZVelocity, lifeTime, elasticity)
    , m_sprite(sprite) {};

    cocos2d::Sprite* getSprite() const { return m_sprite; }
    
    void update(const float deltaTime) override;

private:
    cocos2d::Sprite* m_sprite;
};

#endif /* Pseudo3DSprite_h */
