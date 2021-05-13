#include "MovementIntegrator.h"
#include "EntityDataModel.h"
#include "CollisionUtils.h"
#include "SharedConstants.h"
#include "Game/Shared/EntityConstants.h"


std::function<void(const CollisionData collisionData)> MovementIntegrator::s_collisionCallback = nullptr;

void MovementIntegrator::integratePositions(const float deltaTime,
                                            std::map<uint32_t, EntitySnapshot>& snapshot,
                                            const std::vector<cocos2d::Rect>& staticRects,
                                            std::function<void(const CollisionData collisionData)> collisionCallback)
{
    s_collisionCallback = collisionCallback;
    for (auto& entityPair : snapshot)
    {
        EntitySnapshot& entity = entityPair.second;
        const uint32_t entityID = entityPair.first;
        integratePosition(deltaTime, entityID, entity, snapshot, staticRects);
    }
    s_collisionCallback = nullptr;
}

void MovementIntegrator::integratePosition(const float deltaTime,
                                           const uint32_t entityID,
                                           EntitySnapshot& entity,
                                           const std::map<uint32_t, EntitySnapshot>& snapshot,
                                           const std::vector<cocos2d::Rect>& staticRects)
{
    const bool useContinuousCollisionDetection = true;

    uint16_t collisionEntityID = 0;
    size_t collisionShapeIndex = 0;
    bool isStaticCollision = false;
    
    if (entity.angularVelocity != 0.f)
    {
        entity.rotation = entity.rotation + (entity.angularVelocity * deltaTime);
    }
    
    const cocos2d::Vec2 velocity = cocos2d::Vec2(entity.velocityX, entity.velocityY);
    if (velocity == cocos2d::Vec2::ZERO)
    {
        return;
    }
    
    const cocos2d::Vec2 originalPosition = cocos2d::Vec2(entity.positionX, entity.positionY);
    
    if (!useContinuousCollisionDetection)
    {
        const cocos2d::Vec2 movement = velocity * deltaTime;
        entity.positionX = originalPosition.x + movement.x;
        entity.positionY = originalPosition.y + movement.y;
    }
    
    float movementRatio = 1.f;
    // Check collision rect against all other entities
    for (const auto& collisionEntityPair : snapshot)
    {
        if (entityID == collisionEntityPair.first)
        {
            continue; // No need to test against own shapes
        }
        
        const EntitySnapshot& colliderEntity = collisionEntityPair.second;
        if ((colliderEntity.type > EntityType::Item_First_Placeholder &&
            colliderEntity.type < EntityType::Item_Last_Placeholder) ||
            colliderEntity.type == EntityType::Loot_Box)
        {
            continue; // No collisions against items
        }
        
        size_t shapeIndex = 0;
        // For each collision rect in this potential collider entity
        const auto& colliderRects = EntityDataModel::getCollisionRects((EntityType)colliderEntity.type);
        for (const cocos2d::Rect& baseColliderRect : colliderRects)
        {
            const cocos2d::Vec2 colliderPos = cocos2d::Vec2(colliderEntity.positionX, colliderEntity.positionY);
            const cocos2d::Vec2 colliderVel = cocos2d::Vec2(colliderEntity.velocityX, colliderEntity.velocityY);
            const cocos2d::Rect colliderRect = cocos2d::Rect(baseColliderRect.origin + colliderPos, baseColliderRect.size);
            const float newRatio = getMovementRatio(entity, colliderRect, colliderVel, deltaTime);
            if (newRatio < movementRatio)
            {
                movementRatio = newRatio;
                collisionEntityID = collisionEntityPair.first;
                collisionShapeIndex = shapeIndex;
            }
            shapeIndex++;
            if (entity.type == EntityType::PlayerEntity &&
                colliderEntity.type == EntityType::PlayerEntity)
            {
                // For collisions between players we only consider the feet, that's the first shape
                break;
            }
        }
    }
    
    size_t staticColliderID = 0;
    for (const auto& staticRect : staticRects)
    {
        const float newRatio = getMovementRatio(entity, staticRect, cocos2d::Vec2::ZERO, deltaTime);
        if (newRatio < movementRatio)
        {
            movementRatio = newRatio;
            isStaticCollision = true;
            collisionShapeIndex = staticColliderID;
        }
        staticColliderID++;
    }
    
    if (useContinuousCollisionDetection &&
        movementRatio > 0.f)
    {
        const cocos2d::Vec2 newPos = originalPosition + (velocity * deltaTime * movementRatio);
        entity.positionX = newPos.x;
        entity.positionY = newPos.y;
    }
    
    if ((entity.type == Projectile_Bullet ||
         entity.type == Projectile_Rocket) &&
        movementRatio < 1.f &&
        s_collisionCallback)
    {
        const CollisionData collision = {
            (uint16_t)entityID, collisionEntityID, isStaticCollision, collisionShapeIndex
        };
        s_collisionCallback(collision);
    }
}

float MovementIntegrator::getMovementRatio(EntitySnapshot& entity,
                                           const cocos2d::Rect& colliderRect,
                                           const cocos2d::Vec2& colliderVelocity,
                                           const float deltaTime)
{
    const auto& rects = EntityDataModel::getCollisionRects((EntityType)entity.type);
    if (rects.empty())
    {
        return 1.f;
    }
    const cocos2d::Vec2 velocity = cocos2d::Vec2(entity.velocityX, entity.velocityY);
    const cocos2d::Vec2 originalPosition = cocos2d::Vec2(entity.positionX, entity.positionY);
    
    // Solve collisions by finding smallest movementRatio possible
//    for (const cocos2d::Rect& baseRect : entity->getCollisionRects())
    // For this use case we only collide the ground against the bottom-most shape
    const auto& baseRect = rects.at(0);
    {
        const cocos2d::Rect rect = cocos2d::Rect(baseRect.origin + originalPosition, baseRect.size);
        
        const cocos2d::Rect minkowskiDifference = CollisionUtils::minkowskiDifference(colliderRect, rect);
        if (minkowskiDifference.getMinX() < 0.f &&
            minkowskiDifference.getMaxX() > 0.f &&
            minkowskiDifference.getMinY() < 0.f &&
            minkowskiDifference.getMaxY() > 0.f)
        {
            // AABBs already intersect, use discrete collision detection / separation
            const cocos2d::Vec2 penetrationVector = CollisionUtils::closestPointOnBoundsToPoint(cocos2d::Vec2::ZERO, minkowskiDifference);
            const cocos2d::Vec2 newPosition = originalPosition - penetrationVector;
            entity.positionX = newPosition.x;
            entity.positionY = newPosition.y;
            return 0.f;
        }
        else
        {
            // Use swept collision detection
            const cocos2d::Vec2 relativeMotion = (colliderVelocity - velocity) * deltaTime;
            if (relativeMotion.x == 0.f && relativeMotion.y == 0.f)
            {
                return 1.f;
            }
            // ray-cast the relativeMotion vector against the Minkowski AABB
            float h = CollisionUtils::getRayIntersectionFraction(cocos2d::Vec2::ZERO, relativeMotion, minkowskiDifference);
            if (h != std::numeric_limits<float>::max())
            {
                return h;
            }
        }
    }
    
    return 1.f;
}
