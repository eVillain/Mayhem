#ifndef InputController_h
#define InputController_h

#include "cocos2d.h"
#include "base/CCEventListenerController.h"

class InputModel;
class GLFWJoystickHandler;

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
    void setupControllerListener();
    void update(float deltaTime);

    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

    void onMouseMoved(cocos2d::EventMouse* event);
    void onMouseDown(cocos2d::EventMouse* event);
    void onMouseUp(cocos2d::EventMouse* event);
    
    void onControllerConnected(const int joystickID, const std::string& name);
    void onControllerDisconnected(const int joystickID);
    void onControllerButton(const int joystickID, int button, const float value);
    void onControllerAxis(const int joystickID, int axis, const float value);

    void setActiveInput(const int inputID);

    std::shared_ptr<InputModel> m_model;
    std::shared_ptr<GLFWJoystickHandler> m_joystickHandler;
    cocos2d::Node* m_parent;
    cocos2d::EventListenerKeyboard* m_keyListener;
    cocos2d::EventListenerMouse* m_mouseListener;
    cocos2d::EventListenerController* m_controllerListener;
    cocos2d::Vec2 m_prevMouseCoord;
};

#endif /* InputController_h */
