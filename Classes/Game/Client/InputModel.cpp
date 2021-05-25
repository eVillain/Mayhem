#include "Game/Client/InputModel.h"

#include "InputConstants.h"

InputModel::InputModel()
: m_mouseCoord(cocos2d::Vec2::ZERO)
, m_changeWeapon(false)
, m_slot(0)
, m_pickupType(0)
, m_pickupAmount(0)
, m_pickupID(0)
, m_activeController(-1)
{
    printf("InputModel:: constructor: %p\n", this);
    mapKeyboard(cocos2d::EventKeyboard::KeyCode::KEY_A, InputConstants::ACTION_MOVE_RIGHT, -1.f);
    mapKeyboard(cocos2d::EventKeyboard::KeyCode::KEY_D, InputConstants::ACTION_MOVE_RIGHT, 1.f);
    mapKeyboard(cocos2d::EventKeyboard::KeyCode::KEY_W, InputConstants::ACTION_MOVE_UP, 1.f);
    mapKeyboard(cocos2d::EventKeyboard::KeyCode::KEY_S, InputConstants::ACTION_MOVE_UP, -1.f);
    mapKeyboard(cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW, InputConstants::ACTION_MOVE_RIGHT, -1.f);
    mapKeyboard(cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW, InputConstants::ACTION_MOVE_RIGHT, 1.f);
    mapKeyboard(cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW, InputConstants::ACTION_MOVE_UP, 1.f);
    mapKeyboard(cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW, InputConstants::ACTION_MOVE_UP, -1.f);
    mapKeyboard(cocos2d::EventKeyboard::KeyCode::KEY_SHIFT, InputConstants::ACTION_RUN, 1.f);
    mapKeyboard(cocos2d::EventKeyboard::KeyCode::KEY_F, InputConstants::ACTION_INTERACT, 1.f);
    mapKeyboard(cocos2d::EventKeyboard::KeyCode::KEY_R, InputConstants::ACTION_RELOAD, 1.f);
    mapKeyboard(cocos2d::EventKeyboard::KeyCode::KEY_ESCAPE, InputConstants::ACTION_BACK, 1.f);
    mapKeyboard(cocos2d::EventKeyboard::KeyCode::KEY_COMMA, InputConstants::ACTION_ZOOM_IN, 1.f);
    mapKeyboard(cocos2d::EventKeyboard::KeyCode::KEY_PERIOD, InputConstants::ACTION_ZOOM_OUT, 1.f);
    
    mapMouseButton(cocos2d::EventMouse::MouseButton::BUTTON_LEFT, InputConstants::ACTION_SHOOT, 1.f);
    mapMouseButton(cocos2d::EventMouse::MouseButton::BUTTON_RIGHT, InputConstants::ACTION_AIM, 1.f);
}

InputModel::~InputModel()
{
    printf("InputModel:: destructor: %p\n", this);
}

void InputModel::setInputValue(const std::string& input, const float value)
{
    m_inputs[input] = value;
}

float InputModel::getInputValue(const std::string& input)
{
    if (m_inputs.find(input) == m_inputs.end())
    {
        m_inputs[input] = 0.f;
    }
    return m_inputs[input];
}


void InputModel::mapKeyboard(const cocos2d::EventKeyboard::KeyCode key,
                             const std::string& action,
                             const float value)
{
    m_keyboardMappings[key] = {action, value};
}

void InputModel::mapMouseButton(const cocos2d::EventMouse::MouseButton button,
                                const std::string& action,
                                const float value)
{
    m_mouseButtonMappings[button] = {action, value};
}

void InputModel::mapMouseAxis(const int axis,
                              const std::string& action,
                              const float value)
{
    m_mouseAxisMappings[axis] = {action, value};
}

void InputModel::mapControllerButton(const int button,
                                     const std::string& action,
                                     const float value)
{
    m_controllerButtonMappings[button] = {action, value};
}

void InputModel::mapControllerAxis(const int axis,
                                   const std::string& action,
                                   const float value)
{
    m_controllerAxisMappings[axis] = {action, value};
}
