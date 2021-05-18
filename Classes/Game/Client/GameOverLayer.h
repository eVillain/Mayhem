#ifndef GameOverLayer_h
#define GameOverLayer_h

#include "ViewLayer.h"
#include "cocos2d.h"

class InputModel;

class GameOverLayer : public ViewLayer
{
public:
    static const std::string DESCRIPTOR;
    
    GameOverLayer();
    ~GameOverLayer();
    
    void setup(const std::string& titleText,
               const std::string& text);
    
    void update(const float deltaTime) override {};
    
    cocos2d::Node* getRoot() override { return m_layer; }
    const std::string& getDescriptor() const override { return DESCRIPTOR; }
    
    const cocos2d::RefPtr<cocos2d::Label>& getTextLabel() const { return m_textLabel; }
    const cocos2d::RefPtr<cocos2d::Label>& getTitleLabel() const { return m_titleLabel; }
    const cocos2d::RefPtr<cocos2d::ui::Button>& getExitButton() const { return m_exitButton; }

private:
    cocos2d::RefPtr<cocos2d::LayerColor> m_layer;
    cocos2d::RefPtr<cocos2d::Label> m_textLabel;
    cocos2d::RefPtr<cocos2d::Label> m_titleLabel;
    cocos2d::RefPtr<cocos2d::ui::Button> m_exitButton;
};

#endif /* GameOverLayer_h */
