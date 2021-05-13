#include "EntityDataModel.h"

std::map<const int, StaticEntityData> EntityDataModel::g_staticEntityData;

const StaticEntityData& EntityDataModel::getStaticEntityData(const EntityType type)
{
    auto it = g_staticEntityData.find(type);
    if (it != g_staticEntityData.end())
    {
        return g_staticEntityData[type];
    }
    else
    {
        cocos2d::log("EntityDataModel fail: no such item data for type %i", type);
    }
    return g_staticEntityData.begin()->second;
}

const std::vector<cocos2d::Rect>& EntityDataModel::getCollisionRects(const EntityType type)
{
    if (type == EntityType::PlayerEntity)
    {
        static cocos2d::Rect topShape = cocos2d::Rect(-5, 0, 10, 7);
        static cocos2d::Rect midShape = cocos2d::Rect(-4, -4, 8, 4);
        static cocos2d::Rect bottomShape = cocos2d::Rect(-4, -8, 8, 4);
        static const std::vector<cocos2d::Rect> collisionRects = {bottomShape, midShape, topShape};
        return collisionRects;
    }
    else if (isItemType(type) || isProjectileType(type))
    {
        const auto& itemData = EntityDataModel::getStaticEntityData(type);
        return itemData.rects;
    }
    static const std::vector<cocos2d::Rect> noRects;
    return noRects;
}

bool EntityDataModel::isItemType(const EntityType type)
{
    return (type > EntityType::Item_First_Placeholder && type < EntityType::Item_Last_Placeholder);
}
bool EntityDataModel::isWeaponType(const EntityType type)
{
    return (type > EntityType::Item_First_Placeholder && type <= EntityType::Item_Nade_Smoke);
}
bool EntityDataModel::isProjectileType(const EntityType type)
{
    return (type >= EntityType::Projectile_Bullet && type <= EntityType::Projectile_Smoke);
}
