#include "InputController.h"
#include "Game/Client/InputModel.h"
#include "Core/Injector.h"
#include "Core/Dispatcher.h"
#include "TogglePhysicsDebugEvent.h"
#include "ToggleInventoryEvent.h"
#include "ToggleClientPredictionEvent.h"
#include "BackButtonPressedEvent.h"

InputController::InputController(std::shared_ptr<InputModel> model)
: m_model(model)
{
}

void InputController::initialize(cocos2d::Node* parent)
{
    setupKeyboardListener(parent->getEventDispatcher());
    setupMouseListener(parent->getEventDispatcher());
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

void InputController::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode,
                                   cocos2d::Event *event)
{
    cocos2d::Vec2 direction = m_model->getDirection();
    
    switch(keyCode)
    {
        case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_A:
            direction.x -= 1.0f;
            m_model->setDirection(direction);
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_D:
            direction.x += 1.0f;
            m_model->setDirection(direction);
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_W:
            direction.y -= 1.0f;
            m_model->setDirection(direction);
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_S:
            direction.y += 1.0f;
            m_model->setDirection(direction);
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_SHIFT:
            m_model->setRun(true);
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_F:
            m_model->setInteract(true);
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_R:
            m_model->setReload(true);
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_Q:
            m_model->setAim(true);
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_COMMA:
            m_model->setZoomIn(true);
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_PERIOD:
            m_model->setZoomOut(true);
            break;
        default:
            break;
    }
}

void InputController::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event)
{
    cocos2d::Vec2 direction = m_model->getDirection();
    
    switch(keyCode)
    {
        case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_A:
            direction.x += 1.0f;
            m_model->setDirection(direction);
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_D:
            direction.x -= 1.0f;
            m_model->setDirection(direction);
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_W:
            direction.y += 1.0f;
            m_model->setDirection(direction);
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_S:
            direction.y -= 1.0f;
            m_model->setDirection(direction);
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_SHIFT:
            m_model->setRun(false);
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_F:
            m_model->setInteract(false);
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_R:
            m_model->setReload(false);
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_COMMA:
            m_model->setZoomIn(false);
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_PERIOD:
            m_model->setZoomOut(false);
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_Q:
            m_model->setAim(false);
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_0:
        case cocos2d::EventKeyboard::KeyCode::KEY_1:
        case cocos2d::EventKeyboard::KeyCode::KEY_2:
        case cocos2d::EventKeyboard::KeyCode::KEY_3:
        case cocos2d::EventKeyboard::KeyCode::KEY_4:
        case cocos2d::EventKeyboard::KeyCode::KEY_5:
        case cocos2d::EventKeyboard::KeyCode::KEY_6:
        case cocos2d::EventKeyboard::KeyCode::KEY_7:
        case cocos2d::EventKeyboard::KeyCode::KEY_8:
        case cocos2d::EventKeyboard::KeyCode::KEY_9:
            m_model->setSlot((int)keyCode - (int)cocos2d::EventKeyboard::KeyCode::KEY_0);
            m_model->setChangeWeapon(true);
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_B:
            Dispatcher::globalDispatcher().dispatch(TogglePhysicsDebugEvent());
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_I:
        case cocos2d::EventKeyboard::KeyCode::KEY_TAB:
            Dispatcher::globalDispatcher().dispatch(ToggleInventoryEvent());
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_N:
            Dispatcher::globalDispatcher().dispatch(ToggleClientPredictionEvent());
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_M:
//            Dispatcher::globalDispatcher().dispatch(ToggleMuteBGMEvent());
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_ESCAPE:
            Dispatcher::globalDispatcher().dispatch(BackButtonPressedEvent());
            break;
        default:
            break;
    }
}

void InputController::onMouseMoved(cocos2d::EventMouse *event)
{
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
    if (event->getMouseButton() == cocos2d::EventMouse::MouseButton::BUTTON_LEFT)
    {
        m_model->setShoot(true);
    }
    else if (event->getMouseButton() == cocos2d::EventMouse::MouseButton::BUTTON_RIGHT)
    {
        m_model->setAim(true);
    }
}

void InputController::onMouseUp(cocos2d::EventMouse *event)
{
    if (event->getMouseButton() == cocos2d::EventMouse::MouseButton::BUTTON_LEFT)
    {
        m_model->setShoot(false);
    }
    else if (event->getMouseButton() == cocos2d::EventMouse::MouseButton::BUTTON_RIGHT)
    {
        m_model->setAim(false);
    }
}

