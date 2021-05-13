#ifndef LoadStaticEntityDataCommand_h
#define LoadStaticEntityDataCommand_h

#include "Core/Command.h"
#include "EntityDataModel.h"

namespace tinyxml2
{
    class XMLElement;
};

class LoadStaticEntityDataCommand : public Command
{
public:
    LoadStaticEntityDataCommand();

    virtual bool run() override;

    std::map<const int, StaticEntityData> itemDataMap;

private:
    void loadItemDefs();

    void loadWeaponData(const tinyxml2::XMLElement* element,
                        WeaponData& data);
    void loadAmmoData(const tinyxml2::XMLElement* element,
                      AmmoData& data);
    void loadArmorData(const tinyxml2::XMLElement* element,
                       ArmorData& data);
    bool getStringFromElement(const std::string& name,
                              const tinyxml2::XMLElement* element,
                              std::string& output);
    
    bool getFloatFromElement(const std::string& name,
                             const tinyxml2::XMLElement* element,
                             float& output);
    
    bool getIntFromElement(const std::string& name,
                           const tinyxml2::XMLElement* element,
                           int& output);
    
    bool getVec2FromElement(const std::string& name,
                            const tinyxml2::XMLElement* element,
                            cocos2d::Vec2& output);
    
    bool getRectFromElement(const std::string& name,
                            const tinyxml2::XMLElement* element,
                            cocos2d::Rect& output);
};

#endif /* LoadStaticEntityDataCommand_h */
