#include "LoadStaticEntityDataCommand.h"
#include "external/tinyxml2/tinyxml2.h"

const std::map<std::string, WeaponType> WEAPON_TYPES =
{
    {"pistol", Weapon_Type_Pistol},
    {"smg", Weapon_Type_SMG},
    {"ar", Weapon_Type_AR},
    {"shotgun", Weapon_Type_Shotgun},
    {"sniper", Weapon_Type_Sniper},
    {"launcher", Weapon_Type_Launcher},
    {"throwable", Weapon_Type_Throwable},
};
const std::map<std::string, DamageType> DAMAGE_TYPES =
{
    {"none", Damage_Type_None},
    {"hitscan", Damage_Type_Hitscan},
    {"projectile", Damage_Type_Projectile},
    {"explosion", Damage_Type_Explosion},
};
const std::map<const std::string, const AmmoType> AMMO_TYPES =
{
    {"9MM", Ammo_Type_9mm},
    {"45ACP", Ammo_Type_45ACP},
    {"556", Ammo_Type_556},
    {"762", Ammo_Type_762},
    {"Shells", Ammo_Type_Shells},
    {"RPG Ammo", Ammo_Type_RPG},
    {"FRAG", Ammo_Type_Frag},
    {"SMOKE", Ammo_Type_Smoke},
    {"Slugs", Ammo_Type_Slug}
};
const std::map<std::string, ArmorType> ARMOR_TYPES =
{
    {"helmet", ArmorType::Armor_Type_Helmet},
    {"vest", ArmorType::Armor_Type_Vest},
};

LoadStaticEntityDataCommand::LoadStaticEntityDataCommand()
{
}

bool LoadStaticEntityDataCommand::run()
{
    std::string fullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename("res/EntityDefs.xml");
    tinyxml2::XMLDocument xmlDoc;
    tinyxml2::XMLError eResult = xmlDoc.LoadFile(fullPath.c_str());
    if (eResult != tinyxml2::XMLError::XML_SUCCESS)
    {
        cocos2d::log("EntitiesController::loadItemDefs - Failed to load xml file!");
        return false;
    }
    
    tinyxml2::XMLNode * pRoot = xmlDoc.FirstChild();
    if (pRoot == nullptr)
    {
        cocos2d::log("EntitiesController::loadItemDefs - Malformed xml file!");
        return false;
    }
    
    auto entitiesElement = xmlDoc.FirstChildElement("entities");
    auto entityElement = entitiesElement->FirstChildElement("entity");
    while (entityElement != nullptr)
    {
        EntityType typeAttribute;
        
        eResult = entityElement->QueryIntAttribute("type", (int*)&typeAttribute);
        if (eResult != tinyxml2::XMLError::XML_SUCCESS)
        {
            entityElement = entityElement->NextSiblingElement("entity");
            cocos2d::log("EntitiesController::loadItemDefs - no type for item in XML!");
            continue;
        }

        StaticEntityData& data = itemDataMap[typeAttribute];

        getStringFromElement("name", entityElement, data.name);
        getStringFromElement("sprite", entityElement, data.sprite);
        cocos2d::Rect rect;
        getRectFromElement("rect", entityElement, rect);
        data.rects = {rect};
        getVec2FromElement("anchor", entityElement, data.anchor);
        
        auto weaponElement = entityElement->FirstChildElement("weapon");
        if (weaponElement != nullptr)
        {
            loadWeaponData(weaponElement, data.weapon);
        }
        auto ammoElement = entityElement->FirstChildElement("ammo");
        if (ammoElement != nullptr)
        {
            loadAmmoData(ammoElement, data.ammo);
        }
        auto armorElement = entityElement->FirstChildElement("armor");
        if (armorElement != nullptr)
        {
            loadArmorData(armorElement, data.armor);
        }
        
        entityElement = entityElement->NextSiblingElement("entity");
    }
    cocos2d::log("entities loaded: %lu", itemDataMap.size());
    
    return true;
}

void LoadStaticEntityDataCommand::loadWeaponData(const tinyxml2::XMLElement* weaponElement,
                                                 WeaponData& data)
{
    std::string weaponType;
    getStringFromElement("type", weaponElement, weaponType);
    if (WEAPON_TYPES.count(weaponType))
    {
        data.type = WEAPON_TYPES.at(weaponType);
    }
    getStringFromElement("sound", weaponElement, data.sound);
    getVec2FromElement("holdOffset", weaponElement, data.holdOffset);
    getVec2FromElement("projectileOffset", weaponElement, data.projectileOffset);
    
    auto damageElement = weaponElement->FirstChildElement("damage");
    if (damageElement != nullptr)
    {
        std::string damageType;
        getStringFromElement("type", damageElement, damageType);
        if (DAMAGE_TYPES.count(damageType))
        {
            data.damageType = DAMAGE_TYPES.at(damageType);
            getIntFromElement("amount", damageElement, data.damageAmount);
        }
        if (data.damageType == DamageType::Damage_Type_Projectile)
        {
            int projectileType = 0;
            getIntFromElement("projectileType", damageElement, projectileType);
            data.projectileType = (EntityType)projectileType;
        }
    }
    auto recoilElement = weaponElement->FirstChildElement("recoil");
    if (recoilElement != nullptr)
    {
        getFloatFromElement("amount", recoilElement, data.recoil);
    }
    auto timeElement = weaponElement->FirstChildElement("time");
    if (timeElement != nullptr)
    {
        getFloatFromElement("shot", timeElement, data.timeShot);
        getFloatFromElement("reload", timeElement, data.timeReload);
    }
    auto fireModeElement = weaponElement->FirstChildElement("fireMode");
    if (fireModeElement != nullptr)
    {
        int mode;
        getIntFromElement("single", fireModeElement, mode);
        if (mode == 1)
        {
            data.fireModes.push_back(FireModeType::Fire_Mode_Type_Single);
        }
        getIntFromElement("burst", fireModeElement, mode);
        if (mode == 1)
        {
            data.fireModes.push_back(FireModeType::Fire_Mode_Type_Burst);
        }
        getIntFromElement("auto", fireModeElement, mode);
        if (mode == 1)
        {
            data.fireModes.push_back(FireModeType::Fire_Mode_Type_Auto);
        }
    }
}

void LoadStaticEntityDataCommand::loadAmmoData(const tinyxml2::XMLElement* element,
                                               AmmoData& data)
{
    std::string ammoType;
    getStringFromElement("type", element, ammoType);
    if (AMMO_TYPES.count(ammoType))
    {
        data.type = AMMO_TYPES.at(ammoType);
    }
    getIntFromElement("amount", element, data.amount);
}

void LoadStaticEntityDataCommand::loadArmorData(const tinyxml2::XMLElement* element,
                                                ArmorData& data)
{
    std::string armorType;
    getStringFromElement("type", element, armorType);
    if (ARMOR_TYPES.count(armorType))
    {
        data.type = ARMOR_TYPES.at(armorType);
    }
    getIntFromElement("level", element, data.level);
    getIntFromElement("amount", element, data.amount);
}

bool LoadStaticEntityDataCommand::getStringFromElement(const std::string& name,
                                                       const tinyxml2::XMLElement* element,
                                                       std::string& output)
{
    const char* szAttributeText = element->Attribute(name.c_str());
    if (szAttributeText != nullptr)
    {
        output = szAttributeText;
        return true;
    }
    return false;
}

bool LoadStaticEntityDataCommand::getFloatFromElement(const std::string& name,
                                                      const tinyxml2::XMLElement* element,
                                                      float& output)
{
    tinyxml2::XMLError eResult;
    eResult = element->QueryFloatAttribute(name.c_str(), &output);
    return eResult == tinyxml2::XMLError::XML_SUCCESS;
}

bool LoadStaticEntityDataCommand::getIntFromElement(const std::string& name,
                                                    const tinyxml2::XMLElement* element,
                                                    int& output)
{
    tinyxml2::XMLError eResult;
    eResult = element->QueryIntAttribute(name.c_str(), &output);
    return eResult == tinyxml2::XMLError::XML_SUCCESS;
}

bool LoadStaticEntityDataCommand::getVec2FromElement(const std::string& name,
                                                     const tinyxml2::XMLElement* element,
                                                     cocos2d::Vec2& output)
{
    tinyxml2::XMLError eResult;
    auto vec2Element = element->FirstChildElement(name.c_str());
    if (vec2Element != nullptr)
    {
        float xAttribute, yAttribute;
        eResult = vec2Element->QueryFloatAttribute("x", &xAttribute);
        eResult = vec2Element->QueryFloatAttribute("y", &yAttribute);
        output = cocos2d::Vec2(xAttribute, yAttribute);
        return true;
    }
    return false;
}

bool LoadStaticEntityDataCommand::getRectFromElement(const std::string& name,
                                                     const tinyxml2::XMLElement* element,
                                                     cocos2d::Rect& output)
{
    tinyxml2::XMLError eResult;
    auto rectElement = element->FirstChildElement(name.c_str());
    if (rectElement != nullptr)
    {
        float xAttribute, yAttribute;
        float widthAttribute, heightAttribute;
        eResult = rectElement->QueryFloatAttribute("x", &xAttribute);
        eResult = rectElement->QueryFloatAttribute("y", &yAttribute);
        eResult = rectElement->QueryFloatAttribute("width", &widthAttribute);
        eResult = rectElement->QueryFloatAttribute("height", &heightAttribute);
        output = cocos2d::Rect(xAttribute, yAttribute, widthAttribute, heightAttribute);
        return true;
    }
    return false;
}
