#include "ExitGameLayer.h"

#include "HUDHelper.h"
#include "ButtonUtils.h"
#include "GameViewConstants.h"

const std::string ExitGameLayer::DESCRIPTOR = "ExitGameLayer";

ExitGameLayer::ExitGameLayer()
: m_layer(nullptr)
, m_textLabel(nullptr)
, m_titleLabel(nullptr)
, m_confirmButton(nullptr)
, m_cancelButton(nullptr)
{
}

ExitGameLayer::~ExitGameLayer()
{
}

void ExitGameLayer::setup(const std::string& text,
                          const std::string& titleText)
{
    m_layer = cocos2d::LayerColor::create(cocos2d::Color4B(0, 0, 0, 192));

    const cocos2d::Size layerSize = m_layer->getContentSize();
    const cocos2d::Vec2 center = layerSize * 0.5f;
    m_titleLabel = HUDHelper::createLabel5x7(titleText);
    m_titleLabel->setPosition(cocos2d::Vec2(center.x,
                                            layerSize.height * 0.66f));
    m_titleLabel->enableOutline(cocos2d::Color4B::BLACK, 1.f);
    
    m_textLabel = HUDHelper::createLabel5x7(text);
    m_textLabel->enableOutline(cocos2d::Color4B::BLACK, 1.f);
    m_textLabel->setPosition(center);
    m_confirmButton = ButtonUtils::createButton(cocos2d::Vec2(center.x + GameViewConstants::BUTTON_SIZE_CONFIRM_CANCEL.width,
                                                              layerSize.height * 0.33f),
                                                GameViewConstants::BUTTON_SIZE_CONFIRM_CANCEL,
                                                "CONFIRM",
                                                GameViewConstants::FONT_SIZE_MEDIUM);
    m_confirmButton->getTitleLabel()->enableOutline(cocos2d::Color4B::BLACK, 1.f);
    m_confirmButton->setTitleColor(cocos2d::Color3B::GREEN);
    m_cancelButton = ButtonUtils::createButton(cocos2d::Vec2(center.x - GameViewConstants::BUTTON_SIZE_CONFIRM_CANCEL.width,
                                                              layerSize.height * 0.33f),
                                                GameViewConstants::BUTTON_SIZE_CONFIRM_CANCEL,
                                                "CANCEL",
                                                GameViewConstants::FONT_SIZE_MEDIUM);
    m_cancelButton->getTitleLabel()->enableOutline(cocos2d::Color4B::BLACK, 1.f);
    m_cancelButton->setTitleColor(cocos2d::Color3B::RED);
    
    m_layer->addChild(m_textLabel);
    m_layer->addChild(m_titleLabel);
    m_layer->addChild(m_confirmButton);
    m_layer->addChild(m_cancelButton);
}
