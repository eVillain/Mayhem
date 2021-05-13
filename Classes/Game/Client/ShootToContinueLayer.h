#ifndef ShootToContinueLayer_h
#define ShootToContinueLayer_h

#include "ViewLayer.h"
#include "cocos2d.h"

class InputModel;

class ShootToContinueLayer : public ViewLayer
{
public:
    static const std::string DESCRIPTOR;
    
    ShootToContinueLayer(std::shared_ptr<InputModel> inputModel);
    ~ShootToContinueLayer();
    
    void setup(const std::string& text,
               const std::string& continueText,
               std::function<void()> callback);
    
    void update(const float deltaTime) override;
    
    cocos2d::Node* getRoot() override { return m_layer; }
    const std::string& getDescriptor() const override { return DESCRIPTOR; }
    
    const cocos2d::RefPtr<cocos2d::Label>& getTextLabel() const { return m_textLabel; }
    const cocos2d::RefPtr<cocos2d::Label>& getContinueLabel() const { return m_continueLabel; }

private:
    std::shared_ptr<InputModel> m_inputModel;
    cocos2d::RefPtr<cocos2d::LayerColor> m_layer;
    cocos2d::RefPtr<cocos2d::Label> m_textLabel;
    cocos2d::RefPtr<cocos2d::Label> m_continueLabel;
    std::function<void()> m_callback;
};

#endif /* ShootToContinueLayer_h */
