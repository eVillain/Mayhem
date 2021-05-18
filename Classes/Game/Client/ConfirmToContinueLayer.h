#ifndef ConfirmToContinueLayer_h
#define ConfirmToContinueLayer_h

#include "ViewLayer.h"
#include "cocos2d.h"

class ConfirmToContinueLayer : public ViewLayer
{
public:
    static const std::string DESCRIPTOR;
    
    ConfirmToContinueLayer();
    ~ConfirmToContinueLayer();
    
    void setup(const std::string& text,
               const std::string& continueText,
               const std::string& confirmButtonLabel);
    
    void update(const float deltaTime) override {};
    
    cocos2d::Node* getRoot() override { return m_layer; }
    const std::string& getDescriptor() const override { return DESCRIPTOR; }
    
    const cocos2d::RefPtr<cocos2d::Label>& getTextLabel() const { return m_textLabel; }
    const cocos2d::RefPtr<cocos2d::Label>& getContinueLabel() const { return m_continueLabel; }
    const cocos2d::RefPtr<cocos2d::ui::Button>& getConfirmButton() const { return m_confirmButton; }

private:
    cocos2d::RefPtr<cocos2d::LayerColor> m_layer;
    cocos2d::RefPtr<cocos2d::Label> m_textLabel;
    cocos2d::RefPtr<cocos2d::Label> m_continueLabel;
    cocos2d::RefPtr<cocos2d::ui::Button> m_confirmButton;
};

#endif /* ConfirmToContinueLayer_h */
