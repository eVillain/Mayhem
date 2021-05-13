#include "ButtonUtils.h"
#include "HUDHelper.h"
#include "GameViewConstants.h"

cocos2d::ui::Button* ButtonUtils::createButton(const cocos2d::Vec2 position,
                                               const cocos2d::Size size,
                                               const std::string text,
                                               const float fontSize /*= 22.0f*/)
{
    cocos2d::ui::Button* button = cocos2d::ui::Button::create(GameViewConstants::HUD_SCALE9_BUTTON,
                                                              GameViewConstants::HUD_SCALE9_BUTTON_PRESSED,
                                                              GameViewConstants::HUD_SCALE9_BUTTON_INACTIVE);
    button->setPosition(position);
    button->setContentSize(size);
    button->setScale9Enabled(true);
    button->setColor(GameViewConstants::BUTTON_COLOR);
    
    button->setTitleText(text);
    button->setTitleFontName(GameViewConstants::FONT_5X7);
    button->setTitleFontSize(fontSize);
    button->setTitleColor(cocos2d::Color3B::BLACK);
    
    cocos2d::ui::Scale9Sprite* border = HUDHelper::createHUDOutline();
    border->setContentSize(size - cocos2d::Size(4,4));
    border->setPosition(size * 0.5f);
    border->setColor(GameViewConstants::BUTTON_BORDER_COLOR);
    button->addChild(border);
    
    return button;
}
