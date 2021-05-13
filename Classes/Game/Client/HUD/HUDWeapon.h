#ifndef HUD_WEAPON_H
#define HUD_WEAPON_H

#include "cocos2d.h"
#include "cocos-ext.h"

class HUDWeapon : public cocos2d::Node
{
public:
    HUDWeapon();
    
    virtual bool init();
    
    void setup(const std::string& spriteName,
               const bool highlight);
    void refreshPositions();
    
    CREATE_FUNC(HUDWeapon);

    cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> getBG() { return m_bg; }
    cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> getBorder() { return m_border; }
    cocos2d::RefPtr<cocos2d::Sprite> getSprite() { return m_sprite; }

private:
    cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> m_bg;
    cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> m_border;
    cocos2d::RefPtr<cocos2d::Sprite> m_sprite;
};

#endif // HUD_WEAPON_H
