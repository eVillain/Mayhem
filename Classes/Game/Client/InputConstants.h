#ifndef InputConstants_h
#define InputConstants_h

#include "cocos2d.h"
#include <string>

enum GamepadButtons {
    FACE_BOTTOM = 0,
    FACE_RIGHT,
    FACE_LEFT,
    FACE_TOP,
    LEFT_BUMPER,
    RIGHT_BUMPER,
    BACK,
    START,
    GUIDE,
    LEFT_THUMB,
    RIGHT_THUMB,
    DPAD_UP,
    DPAD_RIGHT,
    DPAD_DOWN,
    DPAD_LEFT,
};

enum GamepadAxes {
    LEFT_X = 0,
    LEFT_Y,
    RIGHT_X,
    RIGHT_Y,
    LEFT_TRIGGER,
    RIGHT_TRIGGER,
};

enum GamepadType {
    XBOX_ONE = 0,
    XBOX_SERIESX,
    PLAYSTATION_4,
    PLAYSTATION_5,
    SWITCH
};

namespace InputConstants
{
static const std::string ACTION_MOVE_RIGHT = "MoveRight";
static const std::string ACTION_MOVE_UP = "MoveUp";
static const std::string ACTION_AIM_RIGHT = "AimRight";
static const std::string ACTION_AIM_UP = "AimUp";
static const std::string ACTION_SHOOT = "Shoot";
static const std::string ACTION_AIM = "Aim";
static const std::string ACTION_RUN = "Run";
static const std::string ACTION_RELOAD = "Reload";
static const std::string ACTION_INTERACT = "Interact";
static const std::string ACTION_BACK = "Back";
static const std::string ACTION_ZOOM_IN = "ZoomIn";
static const std::string ACTION_ZOOM_OUT = "ZoomOut";

static const std::string ACTION_INVENTORY = "Inventory";
static const std::string ACTION_RENDER_DEBUG = "RenderDebug";
static const std::string ACTION_CLIENT_PREDICTION = "ClientPrediction";

static const GamepadType determineTypeByName(const std::string& name)
{
    static const std::string XBONE_TYPE_STR = "Xbox One";
    static const std::string XSX_TYPE_STR = "Xbox Series";
    static const std::string PS4_TYPE_STR = "PS4";
    static const std::string PS5_TYPE_STR = "PS5";
    static const std::string SWITCH_TYPE_STR = "Switch";

    if (name.find(XBONE_TYPE_STR) != std::string::npos)
    {
        return GamepadType::XBOX_ONE;
    }
    else if (name.find(XSX_TYPE_STR) != std::string::npos)
    {
        return GamepadType::XBOX_SERIESX;
    }
    else if (name.find(PS4_TYPE_STR) != std::string::npos)
    {
        return GamepadType::PLAYSTATION_4;
    }
    else if (name.find(PS5_TYPE_STR) != std::string::npos)
    {
        return GamepadType::PLAYSTATION_5;
    }
    else if (name.find(SWITCH_TYPE_STR) != std::string::npos)
    {
        return GamepadType::SWITCH;
    }
    return GamepadType::XBOX_ONE;
}

static const std::map<GamepadType, std::string> GAMEPAD_TYPE_SPRITES =
{
    {XBOX_ONE, "XboxOne_Diagram_Simple.png"},
    {XBOX_SERIESX, "XboxSeriesX_Diagram_Simple.png"},
    {PLAYSTATION_4, "PS4_Diagram_Simple.png"},
    {PLAYSTATION_5, "PS5_Diagram_Simple.png"},
    {SWITCH, "Switch_Controllers.png"},
};

static const std::string BLANK_KEY_SPRITE = "Blank_Black_Normal.png";
static const std::map<cocos2d::EventKeyboard::KeyCode, std::string> KEY_SPRITES =
{
     {cocos2d::EventKeyboard::KeyCode::KEY_0, "0_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_1, "1_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_2, "2_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_3, "3_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_4, "4_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_5, "5_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_6, "6_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_7, "7_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_8, "8_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_9, "9_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_A, "A_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_ALT, "Alt_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ALT, "Alt_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW, "Arrow_Down_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW, "Arrow_Left_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW, "Arrow_Right_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW, "Arrow_Up_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_ASTERISK, "Asterisk_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_B, "B_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_BACKSPACE, "Backspace_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_LEFT_BRACKET, "Bracket_Left_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_BRACKET, "Bracket_Right_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_C, "C_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_CAPS_LOCK, "Caps_Lock_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_CTRL, "Ctrl_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_D, "D_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_DELETE, "Del_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_E, "E_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_END, "End_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_ENTER, "Enter_Alt_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_RETURN, "Enter_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_KP_ENTER, "Enter_Tall_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_ESCAPE, "Esc_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_F10, "F10_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_F11, "F11_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_F12, "F12_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_F1, "F1_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_F2, "F2_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_F3, "F3_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_F4, "F4_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_F5, "F5_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_F6, "F6_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_F7, "F7_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_F8, "F8_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_F9, "F9_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_F, "F_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_G, "G_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_H, "H_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_HOME, "Home_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_I, "I_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_INSERT, "Insert_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_J, "J_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_K, "K_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_L, "L_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_M, "M_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_MINUS, "Minus_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_N, "N_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_NUM_LOCK, "Num_Lock_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_O, "O_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_P, "P_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_PG_UP, "Page_Down_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_PG_DOWN, "Page_Up_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_PLUS, "Plus_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_KP_PLUS, "Plus_Tall_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_PRINT, "Print_Screen_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_Q, "Q_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_QUESTION, "Question_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_QUOTE, "Quote_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_R, "R_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_S, "S_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_SEMICOLON, "Semicolon_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_SHIFT, "Shift_Alt_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_SHIFT, "Shift_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_SLASH, "Slash_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_SPACE, "Space_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_T, "T_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_TAB, "Tab_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_TILDE, "Tilda_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_U, "U_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_V, "V_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_W, "W_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_X, "X_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_Y, "Y_Key_Dark.png"},
     {cocos2d::EventKeyboard::KeyCode::KEY_Z, "Z_Key_Dark.png"},
// Unused:
//     {cocos2d::EventKeyboard::KeyCode::KEY_, "Backspace_Alt_Key_Dark.png"},
//     {cocos2d::EventKeyboard::KeyCode::KEY_, "Command_Key_Dark.png"},
//     {cocos2d::EventKeyboard::KeyCode::KEY_, "Mark_Left_Key_Dark.png"},
//     {cocos2d::EventKeyboard::KeyCode::KEY_, "Mark_Right_Key_Dark.png"},
//     {cocos2d::EventKeyboard::KeyCode::KEY_, "Win_Key_Dark.png"},
};

static const std::string BLANK_MOUSE_SPRITE = "Mouse_Simple_Key_Dark.png";
static const std::map<cocos2d::EventMouse::MouseButton, std::string> MOUSE_SPRITES =
{
    {cocos2d::EventMouse::MouseButton::BUTTON_LEFT, "Mouse_Left_Key_Dark.png"},
    {cocos2d::EventMouse::MouseButton::BUTTON_MIDDLE, "Mouse_Middle_Key_Dark.png"},
    {cocos2d::EventMouse::MouseButton::BUTTON_RIGHT, "Mouse_Right_Key_Dark.png"},
};

static const std::map<GamepadButtons, std::string> GAMEPAD_XONE_BUTTON_SPRITES =
{
    {GamepadButtons::FACE_BOTTOM, "XboxOne_A.png"},
    {GamepadButtons::FACE_RIGHT, "XboxOne_B.png"},
    {GamepadButtons::FACE_LEFT, "XboxOne_X.png"},
    {GamepadButtons::FACE_TOP, "XboxOne_Y.png"},
    {GamepadButtons::DPAD_DOWN, "XboxOne_Dpad_Down.png"},
    {GamepadButtons::DPAD_RIGHT, "XboxOne_Dpad_Right.png"},
    {GamepadButtons::DPAD_LEFT, "XboxOne_Dpad_Left.png"},
    {GamepadButtons::DPAD_UP, "XboxOne_Dpad_Up.png"},
    {GamepadButtons::LEFT_BUMPER, "XboxOne_LB.png"},
    {GamepadButtons::RIGHT_BUMPER, "XboxOne_RB.png"},
    {GamepadButtons::LEFT_THUMB, "XboxOne_Left_Stick_Click.png"},
    {GamepadButtons::RIGHT_THUMB, "XboxOne_Right_Stick_Click.png"},
    {GamepadButtons::BACK, "XboxOne_Menu.png"},
    {GamepadButtons::START, "XboxOne_Windows.png"},
    {GamepadButtons::GUIDE, "XboxOne_Windows.png"},
};

static const std::map<GamepadButtons, std::string> GAMEPAD_XSERIESX_BUTTON_SPRITES =
{
    {GamepadButtons::FACE_BOTTOM, "XboxSeriesX_A.png"},
    {GamepadButtons::FACE_RIGHT, "XboxSeriesX_B.png"},
    {GamepadButtons::FACE_LEFT, "XboxSeriesX_X.png"},
    {GamepadButtons::FACE_TOP, "XboxSeriesX_Y.png"},
    {GamepadButtons::DPAD_DOWN, "XboxSeriesX_Dpad_Down.png"},
    {GamepadButtons::DPAD_RIGHT, "XboxSeriesX_Dpad_Right.png"},
    {GamepadButtons::DPAD_LEFT, "XboxSeriesX_Dpad_Left.png"},
    {GamepadButtons::DPAD_UP, "XboxSeriesX_Dpad_Up.png"},
    {GamepadButtons::LEFT_BUMPER, "XboxSeriesX_LB.png"},
    {GamepadButtons::RIGHT_BUMPER, "XboxSeriesX_RB.png"},
    {GamepadButtons::LEFT_THUMB, "XboxSeriesX_Left_Stick_Click.png"},
    {GamepadButtons::RIGHT_THUMB, "XboxSeriesX_Right_Stick_Click.png"},
    {GamepadButtons::BACK, "XboxSeriesX_Menu.png"},
    {GamepadButtons::START, "XboxSeriesX_View.png"},
    {GamepadButtons::GUIDE, "XboxSeriesX_Share.png"},
};

static const std::map<GamepadButtons, std::string> GAMEPAD_PS4_BUTTON_SPRITES =
{
    {GamepadButtons::FACE_BOTTOM, "PS4_Cross.png"},
    {GamepadButtons::FACE_RIGHT, "PS4_Circle.png"},
    {GamepadButtons::FACE_LEFT, "PS4_Square.png"},
    {GamepadButtons::FACE_TOP, "PS4_Triangle.png"},
    {GamepadButtons::DPAD_DOWN, "PS4_Dpad_Down.png"},
    {GamepadButtons::DPAD_RIGHT, "PS4_Dpad_Right.png"},
    {GamepadButtons::DPAD_LEFT, "PS4_Dpad_Left.png"},
    {GamepadButtons::DPAD_UP, "PS4_Dpad_Up.png"},
    {GamepadButtons::LEFT_BUMPER, "PS4_L1.png"},
    {GamepadButtons::RIGHT_BUMPER, "PS4_R1.png"},
    {GamepadButtons::LEFT_THUMB, "PS4_Left_Stick_Click.png"},
    {GamepadButtons::RIGHT_THUMB, "PS4_Right_Stick_Click.png"},
    {GamepadButtons::BACK, "PS4_Options.png"},
    {GamepadButtons::START, "PS4_Touch_Pad.png"},
    {GamepadButtons::GUIDE, "PS4_Share.png"},
};

static const std::map<GamepadButtons, std::string> GAMEPAD_PS5_BUTTON_SPRITES =
{
    {GamepadButtons::FACE_BOTTOM, "PS5_Cross.png"},
    {GamepadButtons::FACE_RIGHT, "PS5_Circle.png"},
    {GamepadButtons::FACE_LEFT, "PS5_Square.png"},
    {GamepadButtons::FACE_TOP, "PS5_Triangle.png"},
    {GamepadButtons::DPAD_DOWN, "PS5_Dpad_Down.png"},
    {GamepadButtons::DPAD_RIGHT, "PS5_Dpad_Right.png"},
    {GamepadButtons::DPAD_LEFT, "PS5_Dpad_Left.png"},
    {GamepadButtons::DPAD_UP, "PS5_Dpad_Up.png"},
    {GamepadButtons::LEFT_BUMPER, "PS5_L1.png"},
    {GamepadButtons::RIGHT_BUMPER, "PS5_R1.png"},
    {GamepadButtons::LEFT_THUMB, "PS5_Left_Stick_Click.png"},
    {GamepadButtons::RIGHT_THUMB, "PS5_Right_Stick_Click.png"},
    {GamepadButtons::BACK, "PS5_Options.png"},
    {GamepadButtons::START, "PS5_Touch_Pad.png"},
    {GamepadButtons::GUIDE, "PS5_Share.png"},
};

static const std::map<GamepadButtons, std::string> GAMEPAD_SWITCH_BUTTON_SPRITES =
{
    {GamepadButtons::FACE_BOTTOM, "Switch_B.png"},
    {GamepadButtons::FACE_RIGHT, "Switch_A.png"},
    {GamepadButtons::FACE_LEFT, "Switch_Y.png"},
    {GamepadButtons::FACE_TOP, "Switch_X.png"},
    {GamepadButtons::DPAD_DOWN, "Switch_Dpad_Down.png"},
    {GamepadButtons::DPAD_RIGHT, "Switch_Dpad_Right.png"},
    {GamepadButtons::DPAD_LEFT, "Switch_Dpad_Left.png"},
    {GamepadButtons::DPAD_UP, "Switch_Dpad_Up.png"},
    {GamepadButtons::LEFT_BUMPER, "Switch_LB.png"},
    {GamepadButtons::RIGHT_BUMPER, "Switch_RB.png"},
    {GamepadButtons::LEFT_THUMB, "Switch_LB.png"},
    {GamepadButtons::RIGHT_THUMB, "Switch_RB.png"},
    {GamepadButtons::BACK, "Switch_Minus.png"},
    {GamepadButtons::START, "Switch_Start.png"},
    {GamepadButtons::GUIDE, "Switch_Square.png"},
};

static const std::string getGamepadTypeSprite(GamepadType type)
{
    auto it = GAMEPAD_TYPE_SPRITES.find(type);
    if (it != GAMEPAD_TYPE_SPRITES.end())
    {
        return it->second;
    }
    return GAMEPAD_TYPE_SPRITES.at(GamepadType::XBOX_ONE);
}

static const std::string getKeyboardSprite(cocos2d::EventKeyboard::KeyCode key)
{
    auto it = KEY_SPRITES.find(key);
    if (it != KEY_SPRITES.end())
    {
        return it->second;
    }
    return BLANK_KEY_SPRITE;
}

static const std::string getMouseSprite(cocos2d::EventMouse::MouseButton button)
{
    auto it = MOUSE_SPRITES.find(button);
    if (it != MOUSE_SPRITES.end())
    {
        return it->second;
    }
    return BLANK_MOUSE_SPRITE;
}

static const std::string getGamepadButtonSprite(GamepadButtons button, GamepadType type)
{
    if (type == GamepadType::XBOX_ONE)
    {
        auto it = GAMEPAD_XONE_BUTTON_SPRITES.find(button);
        if (it != GAMEPAD_XONE_BUTTON_SPRITES.end())
        {
            return it->second;
        }
    }
    else if (type == GamepadType::XBOX_SERIESX)
    {
        auto it = GAMEPAD_XSERIESX_BUTTON_SPRITES.find(button);
        if (it != GAMEPAD_XSERIESX_BUTTON_SPRITES.end())
        {
            return it->second;
        }
    }
    if (type == GamepadType::PLAYSTATION_4)
    {
        auto it = GAMEPAD_PS4_BUTTON_SPRITES.find(button);
        if (it != GAMEPAD_PS4_BUTTON_SPRITES.end())
        {
            return it->second;
        }
    }
    if (type == GamepadType::PLAYSTATION_5)
    {
        auto it = GAMEPAD_PS5_BUTTON_SPRITES.find(button);
        if (it != GAMEPAD_PS5_BUTTON_SPRITES.end())
        {
            return it->second;
        }
    }
    if (type == GamepadType::SWITCH)
    {
        auto it = GAMEPAD_SWITCH_BUTTON_SPRITES.find(button);
        if (it != GAMEPAD_SWITCH_BUTTON_SPRITES.end())
        {
            return it->second;
        }
    }
    return BLANK_KEY_SPRITE;
}

};

#endif /* InputConstants_h */
