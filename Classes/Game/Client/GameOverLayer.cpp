#include "GameOverLayer.h"

#include "HUDHelper.h"
#include "ButtonUtils.h"
#include "GameViewConstants.h"

const std::string GameOverLayer::DESCRIPTOR = "GameOverLayer";

GameOverLayer::GameOverLayer()
: m_layer(nullptr)
, m_textLabel(nullptr)
, m_titleLabel(nullptr)
, m_exitButton(nullptr)
{
}

GameOverLayer::~GameOverLayer()
{
}

void GameOverLayer::setup(const std::string& titleText,
                          const std::string& text)
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
    m_exitButton = ButtonUtils::createButton(cocos2d::Vec2(center.x, layerSize.height * 0.33f),
                                                GameViewConstants::BUTTON_SIZE_CONFIRM_CANCEL,
                                                "EXIT",
                                                GameViewConstants::FONT_SIZE_MEDIUM);
    m_exitButton->getTitleLabel()->enableOutline(cocos2d::Color4B::BLACK, 1.f);
    m_exitButton->setTitleColor(cocos2d::Color3B::GREEN);

    m_layer->addChild(m_textLabel);
    m_layer->addChild(m_titleLabel);
    m_layer->addChild(m_exitButton);
}
