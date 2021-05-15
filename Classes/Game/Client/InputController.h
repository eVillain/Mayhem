#ifndef InputController_h
#define InputController_h

#include "cocos2d.h"
class InputModel;

class InputController
{
public:
    InputController(std::shared_ptr<InputModel> model);

    void initialize(cocos2d::Node* parent);
    
private:
    void setupKeyboardListener(cocos2d::EventDispatcher* dispatcher);
    void setupMouseListener(cocos2d::EventDispatcher* dispatcher);

    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

    void onMouseMoved(cocos2d::EventMouse* event);
    void onMouseDown(cocos2d::EventMouse* event);
    void onMouseUp(cocos2d::EventMouse* event);

    std::shared_ptr<InputModel> m_model;
    
    cocos2d::EventListenerKeyboard* m_keyListener;
    cocos2d::EventListenerMouse* m_mouseListener;
    cocos2d::Vec2 m_prevMouseCoord;
};

#endif /* InputController_h */
