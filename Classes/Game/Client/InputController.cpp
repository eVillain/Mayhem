#include "InputController.h"

#include "base/CCController.h"
#include "Core/Dispatcher.h"
#include "Game/Client/InputActivityEvent.h"
#include "Game/Client/InputConnectedEvent.h"
#include "Game/Client/InputModel.h"
#include "Game/Client/GLFWJoystickHandler.h"

InputController::InputController(std::shared_ptr<InputModel> model)
: m_model(model)
, m_parent(nullptr)
, m_joystickHandler(nullptr)
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
    setupControllerListener();
    m_parent->schedule(std::bind(&InputController::update, this, std::placeholders::_1), "InputController");
}

void InputController::shutdown()
{
    if (m_parent)
    {
        m_parent->getEventDispatcher()->removeEventListener(m_keyListener);
        m_parent->getEventDispatcher()->removeEventListener(m_mouseListener);
        m_parent->unschedule("InputController");
    }
    m_parent = nullptr;
    m_keyListener = nullptr;
    m_mouseListener = nullptr;
    m_joystickHandler = nullptr;
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

void InputController::setupControllerListener()
{
    m_joystickHandler = std::make_shared<GLFWJoystickHandler>();
    m_joystickHandler->setConnectedCallback(std::bind(&InputController::onControllerConnected,
                                                      this, std::placeholders::_1, std::placeholders::_2));
    m_joystickHandler->setDisconnectedCallback(std::bind(&InputController::onControllerDisconnected,
                                                      this, std::placeholders::_1));
}

void InputController::update(float deltaTime)
{
    m_joystickHandler->update();
    const auto& gamepads = m_joystickHandler->getGamepads();
    
    for (const auto& pair : gamepads)
    {
        const GLFWJoystickHandler::Gamepad& gamepad = pair.second;
        // We have to check all gamepads for new inputs to determine if one is active
        for (int axis = 0; axis < 6; axis++)
        {
            onControllerAxis(pair.first, axis, gamepad.axes.at(axis));
        }
        for (int button = 0; button < 15; button++)
        {
            onControllerButton(pair.first, button, gamepad.buttons.at(button));
        }
    }
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
    setActiveInput(-1);
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
        setActiveInput(-1);
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
            m_model->setSlot((int)keyCode - (int)cocos2d::EventKeyboard::KeyCode::KEY_0);
            m_model->setChangeWeapon(true);
            setActiveInput(-1);
            break;
        default:
            break;
    }
}

void InputController::onMouseMoved(cocos2d::EventMouse* event)
{
    // TODO: Considering adding ability to map mouse motion
    // in two ways: delta values or absolute screen coordinates
//    const auto& map = m_model->getMouseAxisMap();
//    const auto& it = map.find(0);
//    if (it != map.end())
//    {
//        const InputAction& action = it->second;
//        m_model->setInputValue(action.action, 1.f * action.value);
//        return;
//    }
//    if (m_model->getActiveGamepad() != -1)
//    {
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

void InputController::onControllerConnected(const int joystickID, const std::string& name)
{
    GamepadType type = InputConstants::determineTypeByName(name);
    InputConnectedEvent event(joystickID, name, type, true);
    Dispatcher::globalDispatcher().dispatch(event);
}

void InputController::onControllerDisconnected(const int joystickID)
{
    if (m_model->getActiveGamepad() == joystickID)
    {
        setActiveInput(-1);
    }
    InputConnectedEvent event(joystickID, "", m_model->getActiveGamepadType(), false);
    Dispatcher::globalDispatcher().dispatch(event);
}

void InputController::onControllerButton(const int joystickID, int button, const float value)
{
    const auto& map = m_model->getControllerButtonMap();
    const auto& it = map.find(button);
    if (it == map.end())
    {
        return;
    }
    const InputAction& action = it->second;
    const float newValue = value * action.value;
    m_model->setInputValue(action.action, newValue);
    if (newValue > 0.9f)
    {
        setActiveInput(joystickID);
    }
}

void InputController::onControllerAxis(const int joystickID, int axis, const float value)
{
    const auto& map = m_model->getControllerAxisMap();
    const auto& it = map.find(axis);
    if (it == map.end())
    {
        return;
    }
    const InputAction& action = it->second;
    const float newValue = value * action.value;
    m_model->setInputValue(action.action, newValue);
    if (newValue > 0.9f)
    {
        setActiveInput(joystickID);
    }
}

void InputController::setActiveInput(const int inputID)
{
    if (inputID != m_model->getActiveGamepad())
    {
        m_model->setActiveGamepad(inputID);
        
        std::string name;
        GamepadType type = GamepadType::XBOX_ONE;
        if (inputID != -1)
        {
            const auto& gamepad = m_joystickHandler->getGamepads().at(inputID);
            name = gamepad.name;
            type = InputConstants::determineTypeByName(name);
        }
        else
        {
            name = "Keyboard+Mouse";
        }
        InputActivityEvent event(inputID, name, type);
        Dispatcher::globalDispatcher().dispatch(event);
    }
}
