#ifndef EntityDataModel_h
#define EntityDataModel_h

#include "Game/Shared/EntityConstants.h"
#include <map>

class EntityDataModel
{
public:
    static const StaticEntityData& getStaticEntityData(const EntityType type);
    static void setStaticEntityData(std::map<const int, StaticEntityData>& data) { g_staticEntityData = std::move(data); }
    static bool hasStaticEntityData() { return !g_staticEntityData.empty(); }
    static const std::vector<cocos2d::Rect>& getCollisionRects(const EntityType type);
    static bool isItemType(const EntityType type);
    static bool isWeaponType(const EntityType type);
    static bool isProjectileType(const EntityType type);
    static bool isAmmoType(const EntityType type);

private:
    static std::map<const int, StaticEntityData> g_staticEntityData;
};

#endif /* EntityDataModel_h */
