#ifndef GameViewConstants_h
#define GameViewConstants_h

#include "cocos2d.h"

namespace GameViewConstants
{
    static const cocos2d::Size BUTTON_SIZE_EXIT = cocos2d::Size(160, 30);
    static const cocos2d::Size BUTTON_SIZE_SEND = cocos2d::Size(50, 20);
    static const cocos2d::Size BUTTON_SIZE_READY = cocos2d::Size(100, 40);
    static const cocos2d::Size BUTTON_SIZE_MENU = cocos2d::Size(160, 40);
    static const cocos2d::Size BUTTON_SIZE_CONFIRM_CANCEL = cocos2d::Size(100, 30);
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
    static const cocos2d::Size EDIT_BOX_SIZE_SETTING = cocos2d::Size(200, 24);
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
