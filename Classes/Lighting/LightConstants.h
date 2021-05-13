#ifndef LightConstants_h
#define LightConstants_h

#include "cocos2d.h"

enum LightType
{
    Light_Point,
};

struct LightData {
    cocos2d::Vec2 position;
    float radius;
    cocos2d::Color4F color;
    float lifeTime;
};

#endif /* LightConstants_h */
