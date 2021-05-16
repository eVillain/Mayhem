#ifndef ExitGameLayer_h
#define ExitGameLayer_h

#include "ViewLayer.h"
#include "cocos2d.h"

class InputModel;

class ExitGameLayer : public ViewLayer
{
public:
    static const std::string DESCRIPTOR;
    
    ExitGameLayer();
    ~ExitGameLayer();
    
    void setup(const std::string& text,
               const std::string& titleText);
    
    void update(const float deltaTime) override {};
    
    cocos2d::Node* getRoot() override { return m_layer; }
    const std::string& getDescriptor() const override { return DESCRIPTOR; }
    
    const cocos2d::RefPtr<cocos2d::Label>& getTextLabel() const { return m_textLabel; }
    const cocos2d::RefPtr<cocos2d::Label>& getTitleLabel() const { return m_titleLabel; }
    cocos2d::ui::Button* getConfirmButton() { return m_confirmButton; }
    cocos2d::ui::Button* getCancelButton() { return m_cancelButton; }

private:
    cocos2d::RefPtr<cocos2d::LayerColor> m_layer;
    cocos2d::RefPtr<cocos2d::Label> m_textLabel;
    cocos2d::RefPtr<cocos2d::Label> m_titleLabel;
    cocos2d::RefPtr<cocos2d::ui::Button> m_confirmButton;
    cocos2d::RefPtr<cocos2d::ui::Button> m_cancelButton;
};

#endif /* ExitGameLayer_h */
