#ifndef RaycastUtil_h
#define RaycastUtil_h

#include "cocos2d.h"
#include "Network/NetworkMessages.h"

class Entity;

struct RayElement {
    uint32_t hitEntityID;
    cocos2d::Vec2 shotStartPoint;
    cocos2d::Vec2 shotEndPoint;
    size_t hitShapeIndex;
};

class RaycastUtil
{
public:
    static RayElement rayCast(const cocos2d::Vec2& start,
                              const cocos2d::Vec2& end,
                              const uint32_t ignoreEntityID,
                              const std::map<uint32_t, std::shared_ptr<Entity>>& entities,
                              const std::vector<cocos2d::Rect>& staticRects);
    
    static RayElement rayCast2(const cocos2d::Vec2& start,
                               const cocos2d::Vec2& end,
                               const uint32_t ignoreEntityID,
                               const std::map<uint32_t, EntitySnapshot>& entities,
                               const std::vector<cocos2d::Rect>& staticRects);
};

#endif /* RaycastUtil_h */
