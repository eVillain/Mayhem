#include "HUDWeapon.h"
#include "GameViewConstants.h"
#include "HUDHelper.h"

USING_NS_CC;

HUDWeapon::HUDWeapon()
: m_bg(nullptr)
, m_border(nullptr)
, m_sprite(nullptr)
{
}

bool HUDWeapon::init()
{
    if (!cocos2d::Node::init())
    {
        return false;
    }
    
    setContentSize(GameViewConstants::HUD_WEAPON_SIZE);

    m_bg = HUDHelper::createHUDFill();
    m_bg->setPreferredSize(GameViewConstants::HUD_WEAPON_SIZE);
    m_bg->setAnchorPoint(cocos2d::Vec2::ZERO);
    m_bg->setColor(GameViewConstants::TABLEVIEW_HEADER_BG_COLOR);
    m_bg->setOpacity(63);
    addChild(m_bg);
    
    m_border = HUDHelper::createHUDOutline();
    m_border->setPreferredSize(GameViewConstants::HUD_WEAPON_SIZE - cocos2d::Size(2,2));
    m_border->setAnchorPoint(cocos2d::Vec2::ZERO);
    m_border->setColor(GameViewConstants::BUTTON_BORDER_COLOR);
    addChild(m_border);
    
    m_sprite = cocos2d::Sprite::create(GameViewConstants::PIXEL_RECT);
    m_sprite->setOpacity(0);
    m_sprite->setScale(2.f);
    addChild(m_sprite);
    
    return true;
}

void HUDWeapon::setup(const std::string& spriteName,
                      const bool highlight)
{
    GLubyte opacity = highlight ? 255 : 127;
    bool showSprite = spriteName.length() > 0;
    if (showSprite)
    {
        m_sprite->setSpriteFrame(spriteName);
        m_sprite->setOpacity(opacity);
    }
    m_sprite->setVisible(showSprite);
    
    m_bg->setOpacity(opacity);

    m_border->setVisible(highlight);
    
    refreshPositions();
}

void HUDWeapon::refreshPositions()
{
    const float PADDING = 8.f;
    const cocos2d::Vec2 SPRITE_POS = cocos2d::Vec2(PADDING + m_sprite->getContentSize().width,
                                                   PADDING + m_sprite->getContentSize().height * 0.5f);

    m_sprite->setPosition(SPRITE_POS);
}
