#ifndef GameViewConstants_h
#define GameViewConstants_h

#include "cocos2d.h"

namespace GameViewConstants
{
    static const std::string FONT_3X6 = "fonts/m3x6.ttf";
    static const std::string FONT_5X7 = "fonts/m5x7.ttf";
    static const std::string PIXEL_RECT = "res/HUD/Scale9-PixelRect.png";
    static const std::string PIXEL_OUTLINE = "res/HUD/Scale9-PixelOutline.png";
    static const std::string PIXEL_FILL = "res/HUD/Scale9-PixelFill.png";
    static const std::string HUD_SPRITE_BATCH_FILE = "res/HUD/HudSprites.plist";
    static const std::string HUD_SPRITE_BATCH_TEXTURE_FILE = "res/HUD/HudSprites.png";
    static const std::string HUD_SCALE9_BUTTON = "res/HUD/Scale9-Button.png";
    static const std::string HUD_SCALE9_BUTTON_PRESSED = "res/HUD/Scale9-ButtonPressed.png";
    static const std::string HUD_SCALE9_BUTTON_INACTIVE = "res/HUD/Scale9-ButtonInactive.png";

    static const cocos2d::Size BUTTON_SIZE_EXIT = cocos2d::Size(160, 30);
    static const cocos2d::Size BUTTON_SIZE_SEND = cocos2d::Size(50, 20);
    static const cocos2d::Size BUTTON_SIZE_READY = cocos2d::Size(100, 40);
    static const cocos2d::Size BUTTON_SIZE_MENU = cocos2d::Size(160, 40);
    static const cocos2d::Size BUTTON_SIZE = cocos2d::Size(50, 30);
    static const float FONT_SIZE_LARGE = 24.f;
    static const float FONT_SIZE_MEDIUM = 20.f;
    static const float FONT_SIZE_SMALL = 18.f;
    static const float FONT_SIZE_SMALLEST = 16.f;
    static const cocos2d::Color3B BUTTON_COLOR = cocos2d::Color3B(127, 127, 160);
    static const cocos2d::Color3B BUTTON_BORDER_COLOR = cocos2d::Color3B(94, 94, 127);


    static const float EDITBOX_DEFAULT_WIDTH = 320.f;
    static const float EDITBOX_DEFAULT_HEIGHT = 20.f;

    static const cocos2d::Size CHAT_BOX_SIZE_MESSAGE = cocos2d::Size(EDITBOX_DEFAULT_WIDTH, 200.f);
    static const cocos2d::Size EDIT_BOX_SIZE_MESSAGE = cocos2d::Size(EDITBOX_DEFAULT_WIDTH - BUTTON_SIZE_SEND.width, EDITBOX_DEFAULT_HEIGHT);
    static const cocos2d::Size EDIT_BOX_HEADER_SIZE_MESSAGE = cocos2d::Size(EDITBOX_DEFAULT_WIDTH, 24);
    static const cocos2d::Color3B EDITBOX_COLOR = cocos2d::Color3B(194, 194, 227);
    
    static const float TABLEVIEW_CELL_DEFAULT_WIDTH = 160;
    static const float TABLEVIEW_CELL_DEFAULT_HEIGHT = 40;
    static const cocos2d::Color3B TABLEVIEW_BG_COLOR = cocos2d::Color3B(64, 64, 92);
    static const cocos2d::Color3B TABLEVIEW_HEADER_BG_COLOR = cocos2d::Color3B(80, 80, 100);

    static const cocos2d::Color3B TABLEVIEW_CELL_COLOR = cocos2d::Color3B(92, 92, 127);
    static const cocos2d::Color3B TABLEVIEW_CELL_COLOR_ALT = cocos2d::Color3B(76, 76, 127);
    static const cocos2d::Size CLIENT_CELL_SIZE = cocos2d::Size(TABLEVIEW_CELL_DEFAULT_WIDTH, TABLEVIEW_CELL_DEFAULT_HEIGHT);
    static const cocos2d::Size HOST_CELL_SIZE = cocos2d::Size(TABLEVIEW_CELL_DEFAULT_WIDTH, TABLEVIEW_CELL_DEFAULT_HEIGHT);
    static const cocos2d::Size GAMEMODE_CELL_SIZE = cocos2d::Size(TABLEVIEW_CELL_DEFAULT_WIDTH, 28);
    static const cocos2d::Size CHAT_MESSAGE_CELL_SIZE = cocos2d::Size(EDITBOX_DEFAULT_WIDTH, 18);
    static const cocos2d::Size SETTING_CELL_SIZE = cocos2d::Size(300, TABLEVIEW_CELL_DEFAULT_HEIGHT);

    static const cocos2d::Size HUD_WEAPON_SIZE = cocos2d::Size(60, 20);

}

#endif /* GameViewConstants_h */
