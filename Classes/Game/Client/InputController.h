#ifndef InputController_h
#define InputController_h

#include "cocos2d.h"
#include "base/CCEventListenerController.h"

class InputModel;

class InputController
{
public:
    InputController(std::shared_ptr<InputModel> model);
    ~InputController();

    void initialize(cocos2d::Node* parent);
    void shutdown();

private:
    void setupKeyboardListener(cocos2d::EventDispatcher* dispatcher);
    void setupMouseListener(cocos2d::EventDispatcher* dispatcher);
    void setupControllerListener(cocos2d::EventDispatcher* dispatcher);

    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

    void onMouseMoved(cocos2d::EventMouse* event);
    void onMouseDown(cocos2d::EventMouse* event);
    void onMouseUp(cocos2d::EventMouse* event);
    
    void onControllerConnected(cocos2d::Controller* controller, cocos2d::Event* event);
    void onControllerDisconnected(cocos2d::Controller* controller, cocos2d::Event* event);
    void onControllerKeyDown(cocos2d::Controller* controller, int key, cocos2d::Event* event);
    void onControllerKeyUp(cocos2d::Controller* controller, int key, cocos2d::Event* event);
    void onControllerKeyRepeat(cocos2d::Controller* controller, int key, cocos2d::Event* event);
    void onControllerAxis(cocos2d::Controller* controller, int axis, cocos2d::Event* event);

    std::shared_ptr<InputModel> m_model;
    cocos2d::Node* m_parent;
    cocos2d::EventListenerKeyboard* m_keyListener;
    cocos2d::EventListenerMouse* m_mouseListener;
    cocos2d::EventListenerController* m_controllerListener;
    cocos2d::Vec2 m_prevMouseCoord;
};

#endif /* InputController_h */
