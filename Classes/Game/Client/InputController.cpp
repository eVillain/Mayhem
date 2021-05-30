#include "InputController.h"

#include "base/CCController.h"
#include "Game/Client/InputModel.h"

InputController::InputController(std::shared_ptr<InputModel> model)
: m_model(model)
, m_parent(nullptr)
{
    printf("InputController:: constructor: %p\n", this);
}

InputController::~InputController()
{
    printf("InputController:: destructor: %p\n", this);
}

void InputController::initialize(cocos2d::Node* parent)
{
    m_parent = parent;
    setupKeyboardListener(parent->getEventDispatcher());
    setupMouseListener(parent->getEventDispatcher());
    setupControllerListener(parent->getEventDispatcher());
}

void InputController::shutdown()
{
    if (m_parent)
    {
        m_parent->getEventDispatcher()->removeEventListener(m_keyListener);
        m_parent->getEventDispatcher()->removeEventListener(m_mouseListener);
        m_parent->getEventDispatcher()->removeEventListener(m_controllerListener);
    }
    m_parent = nullptr;
    m_keyListener = nullptr;
    m_mouseListener = nullptr;
    m_controllerListener = nullptr;
}

void InputController::setupKeyboardListener(cocos2d::EventDispatcher* dispatcher)
{
    m_keyListener = cocos2d::EventListenerKeyboard::create();
    
    m_keyListener->onKeyPressed = CC_CALLBACK_2(InputController::onKeyPressed, this);
    m_keyListener->onKeyReleased = CC_CALLBACK_2(InputController::onKeyReleased, this);

    dispatcher->addEventListenerWithFixedPriority(m_keyListener, 1);
}

void InputController::setupMouseListener(cocos2d::EventDispatcher* dispatcher)
{
    m_mouseListener = cocos2d::EventListenerMouse::create();
    
    m_mouseListener->onMouseMove = CC_CALLBACK_1(InputController::onMouseMoved, this);
    m_mouseListener->onMouseDown = CC_CALLBACK_1(InputController::onMouseDown, this);
    m_mouseListener->onMouseUp = CC_CALLBACK_1(InputController::onMouseUp, this);

    dispatcher->addEventListenerWithFixedPriority(m_mouseListener, 1);
}

void InputController::setupControllerListener(cocos2d::EventDispatcher *dispatcher)
{
    m_controllerListener = cocos2d::EventListenerController::create();
    m_controllerListener->onConnected = std::bind(&InputController::onControllerConnected, this,
                                                  std::placeholders::_1, std::placeholders::_2);
    m_controllerListener->onDisconnected = std::bind(&InputController::onControllerDisconnected, this,
                                                     std::placeholders::_1, std::placeholders::_2);
    m_controllerListener->onKeyDown = std::bind(&InputController::onControllerKeyDown, this,
                                                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    m_controllerListener->onKeyUp = std::bind(&InputController::onControllerKeyUp, this,
                                                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    m_controllerListener->onKeyRepeat = std::bind(&InputController::onControllerKeyRepeat, this,
                                                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    m_controllerListener->onAxisEvent = std::bind(&InputController::onControllerAxis, this,
                                                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

void InputController::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode,
                                   cocos2d::Event *event)
{
    const auto& map = m_model->getKeyboardMap();
    const auto& it = map.find(keyCode);
    if (it == map.end())
    {
        return;
    }
    const InputAction& action = it->second;
    const float value = m_model->getInputValue(action.action);
    m_model->setInputValue(action.action, value + action.value);
}

void InputController::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event)
{
    const auto& map = m_model->getKeyboardMap();
    const auto& it = map.find(keyCode);
    if (it != map.end())
    {
        const InputAction& action = it->second;
        const float value = m_model->getInputValue(action.action);
        m_model->setInputValue(action.action, value - action.value);

        return;
    }
    
    switch(keyCode)
    {
        case cocos2d::EventKeyboard::KeyCode::KEY_0:
        case cocos2d::EventKeyboard::KeyCode::KEY_1:
        case cocos2d::EventKeyboard::KeyCode::KEY_2:
        case cocos2d::EventKeyboard::KeyCode::KEY_3:
        case cocos2d::EventKeyboard::KeyCode::KEY_4:
        case cocos2d::EventKeyboard::KeyCode::KEY_5:
//        case cocos2d::EventKeyboard::KeyCode::KEY_6:
//        case cocos2d::EventKeyboard::KeyCode::KEY_7:
//        case cocos2d::EventKeyboard::KeyCode::KEY_8:
//        case cocos2d::EventKeyboard::KeyCode::KEY_9:
            m_model->setSlot((int)keyCode - (int)cocos2d::EventKeyboard::KeyCode::KEY_0);
            m_model->setChangeWeapon(true);
            break;
        default:
            break;
    }
}

void InputController::onMouseMoved(cocos2d::EventMouse* event)
{
//    const auto& map = m_model->getMouseAxisMap();
//    const auto& it = map.find(0);
//    if (it != map.end())
//    {
//        const InputAction& action = it->second;
//        m_model->setInputValue(action.action, 1.f * action.value);
//        return;
//    }
    const cocos2d::Size winSize = cocos2d::Director::getInstance()->getWinSize();
    const cocos2d::Vec2 delta = event->getLocationInView() - m_prevMouseCoord;
    m_prevMouseCoord = event->getLocationInView();
    cocos2d::Vec2 screenPosition = m_model->getMouseCoord() + delta;
    screenPosition.x = std::max(std::min(screenPosition.x, winSize.width), 0.f);
    screenPosition.y = std::max(std::min(screenPosition.y, winSize.height), 0.f);
    m_model->setMouseCoord(screenPosition);
}

void InputController::onMouseDown(cocos2d::EventMouse* event)
{
    const auto& map = m_model->getMouseButtonMap();
    const auto& it = map.find(event->getMouseButton());
    if (it == map.end())
    {
        return;
    }
    const InputAction& action = it->second;
    m_model->setInputValue(action.action, 1.f * action.value);
}

void InputController::onMouseUp(cocos2d::EventMouse* event)
{
    const auto& map = m_model->getMouseButtonMap();
    const auto& it = map.find(event->getMouseButton());
    if (it == map.end())
    {
        return;
    }
    const InputAction& action = it->second;
    m_model->setInputValue(action.action, 0.f);
}

void InputController::onControllerConnected(cocos2d::Controller* controller, cocos2d::Event* event)
{
    
}

void InputController::onControllerDisconnected(cocos2d::Controller* controller, cocos2d::Event* event)
{
    
}

void InputController::onControllerKeyDown(cocos2d::Controller* controller, int key, cocos2d::Event* event)
{
    const auto& map = m_model->getControllerButtonMap();
    const auto& it = map.find(key);
    if (it != map.end())
    {
        const InputAction& action = it->second;
        m_model->setInputValue(action.action, 1.f * action.value);
        return;
    }
}

void InputController::onControllerKeyUp(cocos2d::Controller* controller, int key, cocos2d::Event* event)
{
    const auto& map = m_model->getControllerButtonMap();
    const auto& it = map.find(key);
    if (it != map.end())
    {
        const InputAction& action = it->second;
        m_model->setInputValue(action.action, 0.f);
        return;
    }
}

void InputController::onControllerKeyRepeat(cocos2d::Controller* controller, int key, cocos2d::Event* event)
{
    const auto& map = m_model->getControllerButtonMap();
    const auto& it = map.find(key);
    if (it != map.end())
    {
        const InputAction& action = it->second;
        m_model->setInputValue(action.action, 1.f * action.value);
        return;
    }
}

void InputController::onControllerAxis(cocos2d::Controller* controller, int axis, cocos2d::Event* event)
{
    const auto& map = m_model->getControllerAxisMap();
    const auto& it = map.find(axis);
    if (it != map.end())
    {
        const InputAction& action = it->second;
        m_model->setInputValue(action.action, controller->getKeyStatus(axis).value * action.value);
        return;
    }
}

