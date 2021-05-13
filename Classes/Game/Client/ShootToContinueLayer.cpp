#include "ShootToContinueLayer.h"
#include "HUDHelper.h"
#include "Game/Client/InputModel.h"

const std::string ShootToContinueLayer::DESCRIPTOR = "ShootToContinue";

ShootToContinueLayer::ShootToContinueLayer(std::shared_ptr<InputModel> inputModel)
: m_inputModel(inputModel)
, m_layer(nullptr)
, m_textLabel(nullptr)
, m_continueLabel(nullptr)
, m_callback(nullptr)
{
}

ShootToContinueLayer::~ShootToContinueLayer()
{
}

void ShootToContinueLayer::setup(const std::string& text,
                                 const std::string& continueText,
                                 std::function<void()> callback)
{
    m_layer = cocos2d::LayerColor::create(cocos2d::Color4B(0,0,0,192));
    m_textLabel = HUDHelper::createLabel5x7(text);
    m_textLabel->enableOutline(cocos2d::Color4B::BLACK, 1.f);
    m_textLabel->setPosition(cocos2d::Vec2(m_layer->getContentSize().width * 0.25f,
                                           m_layer->getContentSize().height * 0.66f));
    
    m_continueLabel = HUDHelper::createLabel5x7(continueText);
    m_continueLabel->setPosition(cocos2d::Vec2(m_layer->getContentSize().width * 0.5f,
                                               m_layer->getContentSize().height * 0.33f));
    m_continueLabel->enableOutline(cocos2d::Color4B::BLACK, 1.f);

    m_layer->addChild(m_textLabel);
    m_layer->addChild(m_continueLabel);
    
    m_callback = callback;
}

void ShootToContinueLayer::update(const float deltaTime)
{
    if (m_inputModel->getShoot() && m_callback)
    {
        m_callback();
    }
}
