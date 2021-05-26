#ifndef HUDConstants_h
#define HUDConstants_h

#include "cocos2d.h"
#include "cocos-ext.h"

namespace HUDConstants
{
    static const float FONT_SIZE_SMALL = 30;
    static const float FONT_SIZE_MEDIUM = 36;
    static const float FONT_SIZE_LARGE = 42;

    static const std::string FONT_3X6 = "fonts/m3x6.ttf";
    static const std::string FONT_5X7 = "fonts/m5x7.ttf";
    static const std::string PIXEL_RECT = "res/hud/Scale9-PixelRect.png";
    static const std::string PIXEL_OUTLINE = "res/hud/Scale9-PixelOutline.png";
    static const std::string PIXEL_FILL = "res/hud/Scale9-PixelFill.png";
    static const std::string HUD_SCALE9_ARROW = "res/hud/Scale9-Arrow.png";
    static const std::string HUD_SCALE9_BUTTON = "res/hud/Scale9-Button.png";
    static const std::string HUD_SCALE9_BUTTON_PRESSED = "res/hud/Scale9-ButtonPressed.png";
    static const std::string HUD_SCALE9_BUTTON_INACTIVE = "res/hud/Scale9-ButtonInactive.png";
    static const std::string HUD_SCALE9_SLIDER_BAR = "res/hud/Scale9-SliderBar.png";
    static const std::string HUD_SCALE9_SLIDER_PROGRESSBAR = "res/hud/Scale9-SliderProgressBar.png";
    static const std::string HUD_SCALE9_SLIDER_BALL = "res/hud/Scale9-SliderBall.png";
    static const std::string HUD_SPRITE_BATCH_FILE = "res/HUD/HudSprites.plist";
    static const std::string HUD_SPRITE_BATCH_TEXTURE_FILE = "res/HUD/HudSprites.png";

    static const cocos2d::Color3B HUD_FILL_COLOR = cocos2d::Color3B(127, 127, 160);
    static const cocos2d::Color3B HUD_BORDER_COLOR = cocos2d::Color3B(255, 255, 255);

    static const cocos2d::Color3B TABLEVIEW_CELL_COLOR = cocos2d::Color3B(92, 92, 127);
    static const cocos2d::Color3B BUTTON_COLOR = cocos2d::Color3B(127, 127, 160);
    static const cocos2d::Color3B BUTTON_BORDER_COLOR = cocos2d::Color3B(94, 94, 127);

    static const cocos2d::Color4F TIMELINE_MEASURE_LINE_COLOR = cocos2d::Color4F(0.25f, 0.25f, 0.25f, 1.f);
    static const cocos2d::Color4F TIMELINE_BEAT_LINE_COLOR = cocos2d::Color4F(0.75f, 0.75f, 0.75f, 0.5f);
    static const cocos2d::Color4F TIMELINE_SUBDIVISION_LINE_COLOR = cocos2d::Color4F(0.25f, 0.25f, 0.25f, 1.f);

    static const cocos2d::Size BORDER_INSET_SIZE = cocos2d::Size(1, 1);
}

#endif /* HUDConstants_h */
