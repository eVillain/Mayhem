#include "RaycastUtil.h"
#include "Entity.h"
#include "CollisionUtils.h"
#include "EntityDataModel.h"

RayElement RaycastUtil::rayCast(const cocos2d::Vec2& start,
                                const cocos2d::Vec2& end,
                                const uint32_t ignoreEntityID,
                                const std::map<uint32_t, std::shared_ptr<Entity>>& entities,
                                const std::vector<cocos2d::Rect>& staticRects)
{
    RayElement result;
    result.hitEntityID = 0;
    result.shotStartPoint = start;
    result.shotEndPoint = end;
    float closestDistance = (end - start).length();
    for (const auto& entityPair : entities)
    {
        if (entityPair.first == ignoreEntityID)
        {
            continue;
        }
        cocos2d::Vec2 entityPosition;
        entityPosition = entityPair.second->getPosition();
        size_t index = 0;
        const auto& rects = EntityDataModel::getCollisionRects(entityPair.second->getEntityType());
        for (const cocos2d::Rect& baseRect : rects)
        {
            const cocos2d::Rect rect = cocos2d::Rect(baseRect.origin + entityPosition, baseRect.size);
            cocos2d::Vec2 collisionPoint;
            
            if (CollisionUtils::lineToRect(start, end, rect, collisionPoint))
            {
                const float distance = (collisionPoint - start).length();
                if (distance < closestDistance)
                {
                    closestDistance = distance;
                    result.shotEndPoint = collisionPoint;
                    result.hitEntityID = entityPair.first;
                    result.hitShapeIndex = index;
                }
            }
            index++;
        }
    }
    
    for (const auto& rect : staticRects)
    {
        cocos2d::Vec2 collisionPoint;

        if (CollisionUtils::lineToRect(start, end, rect, collisionPoint))
        {
            const float distance = (collisionPoint - start).length();
            if (distance < closestDistance)
            {
                closestDistance = distance;
                result.shotEndPoint = collisionPoint;
                result.hitEntityID = 0;
                result.hitShapeIndex = 0;
            }
        }
    }

    return result;
}

RayElement RaycastUtil::rayCast2(const cocos2d::Vec2& start,
                                 const cocos2d::Vec2& end,
                                 const uint32_t ignoreEntityID,
                                 const std::map<uint32_t, EntitySnapshot>& entities,
                                 const std::vector<cocos2d::Rect>& staticRects)
{
    RayElement result;
    result.hitEntityID = 0;
    result.shotStartPoint = start;
    result.shotEndPoint = end;
    float closestDistance = (end - start).length();
    for (const auto& entityPair : entities)
    {
        if (entityPair.first == ignoreEntityID)
        {
            continue;
        }
        cocos2d::Vec2 entityPosition = cocos2d::Vec2(entityPair.second.positionX, entityPair.second.positionY);
        size_t index = 0;
        const auto& rects = EntityDataModel::getCollisionRects((EntityType)entityPair.second.type);
        for (const cocos2d::Rect& baseRect : rects)
        {
            const cocos2d::Rect rect = cocos2d::Rect(baseRect.origin + entityPosition, baseRect.size);
            cocos2d::Vec2 collisionPoint;
            
            if (CollisionUtils::lineToRect(start, end, rect, collisionPoint))
            {
                const float distance = (collisionPoint - start).length();
                if (distance < closestDistance)
                {
                    closestDistance = distance;
                    result.shotEndPoint = collisionPoint;
                    result.hitEntityID = entityPair.first;
                    result.hitShapeIndex = index;
                }
            }
            index++;
        }
    }
    
    for (const auto& rect : staticRects)
    {
        cocos2d::Vec2 collisionPoint;

        if (CollisionUtils::lineToRect(start, end, rect, collisionPoint))
        {
            const float distance = (collisionPoint - start).length();
            if (distance < closestDistance)
            {
                closestDistance = distance;
                result.shotEndPoint = collisionPoint;
                result.hitEntityID = 0;
                result.hitShapeIndex = 0;
            }
        }
    }

    return result;

}
