#ifndef InputView_h
#define InputView_h

#include "cocos2d.h"
class InputActivityEvent;
class InputConnectedEvent;

class InputView
{
public:
    InputView();
    ~InputView();
    cocos2d::RefPtr<cocos2d::Node> getRoot() { return m_root; }
    
private:
    cocos2d::RefPtr<cocos2d::Node> m_root;
    
    void onInputActivity(const InputActivityEvent& event);
    void onInputConnected(const InputConnectedEvent& event);
    
    cocos2d::Node* createBadge() const;
    cocos2d::Label* createLabel(const std::string& text) const;
    cocos2d::Sprite* createSprite(const std::string& frame) const;
};

#endif /* InputView_h */
