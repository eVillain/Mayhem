#ifndef MovementIntegrator_h
#define MovementIntegrator_h

#include "Network/NetworkMessages.h"

struct CollisionData {
    uint16_t entityID;
    uint16_t colliderID;
    bool isStaticCollider;
    size_t hitShapeIndex;
};

class MovementIntegrator
{
public:
    static void setCollisionCallback(std::function<void(const CollisionData collisionData)> cb);
    static void integratePosition(const float deltaTime,
                                  const uint32_t entityID,
                                  EntitySnapshot& entity,
                                  const cocos2d::Vec2& velocity,
                                  const float angularVelocity,
                                  const std::map<uint32_t, EntitySnapshot>& snapshot,
                                  const std::vector<cocos2d::Rect>& staticRects);
    static float getMovementRatio(EntitySnapshot& entity,
                                  const cocos2d::Vec2& velocity,
                                  const cocos2d::Rect& colliderRect,
                                  const cocos2d::Vec2& colliderVelocity,
                                  const float deltaTime);
    
private:
    static std::function<void(const CollisionData collisionData)> s_collisionCallback;
};

#endif /* MovementIntegrator_h */
