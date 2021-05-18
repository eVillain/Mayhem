#include "ConfirmToContinueLayer.h"

#include "HUDHelper.h"
#include "ButtonUtils.h"
#include "GameViewConstants.h"

const std::string ConfirmToContinueLayer::DESCRIPTOR = "ConfirmToContinue";

ConfirmToContinueLayer::ConfirmToContinueLayer()
: m_layer(nullptr)
, m_textLabel(nullptr)
, m_continueLabel(nullptr)
, m_confirmButton(nullptr)
{
}

ConfirmToContinueLayer::~ConfirmToContinueLayer()
{
}

void ConfirmToContinueLayer::setup(const std::string& text,
                                   const std::string& continueText,
                                   const std::string& confirmButtonLabel)
{
    m_layer = cocos2d::LayerColor::create(cocos2d::Color4B(0,0,0,192));
    const cocos2d::Size layerSize = m_layer->getContentSize();
    const cocos2d::Vec2 center = layerSize * 0.5f;
    
    m_textLabel = HUDHelper::createLabel5x7(text);
    m_textLabel->enableOutline(cocos2d::Color4B::BLACK, 1.f);
    m_textLabel->setPosition(cocos2d::Vec2(center.x,
                                           m_layer->getContentSize().height * 0.66f));
    
    m_continueLabel = HUDHelper::createLabel5x7(continueText);
    m_continueLabel->setPosition(cocos2d::Vec2(center.x,
                                               m_layer->getContentSize().height * 0.33f));
    m_continueLabel->enableOutline(cocos2d::Color4B::BLACK, 1.f);
    
    m_confirmButton = ButtonUtils::createButton(cocos2d::Vec2(center.x, layerSize.height * 0.33f),
                                                GameViewConstants::BUTTON_SIZE_CONFIRM_CANCEL,
                                                confirmButtonLabel,
                                                GameViewConstants::FONT_SIZE_MEDIUM);
    m_confirmButton->getTitleLabel()->enableOutline(cocos2d::Color4B::BLACK, 1.f);
    m_confirmButton->setTitleColor(cocos2d::Color3B::GREEN);
    
    m_layer->addChild(m_textLabel);
    m_layer->addChild(m_continueLabel);
    m_layer->addChild(m_confirmButton);
}
