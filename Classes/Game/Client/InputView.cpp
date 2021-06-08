#include "InputView.h"

#include "Dispatcher.h"
#include "InputConstants.h"
#include "InputActivityEvent.h"
#include "InputConnectedEvent.h"
#include "HUDHelper.h"
#include "ActionUtils.h"

InputView::InputView()
: m_root(cocos2d::Node::create())
{
    cocos2d::SpriteFrameCache* cache = cocos2d::SpriteFrameCache::getInstance();
    cache->addSpriteFramesWithFile("res/HUD/UIInputGFX.plist");

    Dispatcher::globalDispatcher().addListener<InputConnectedEvent>(std::bind(&InputView::onInputConnected,
                                                                             this, std::placeholders::_1),
                                                                    this);

    Dispatcher::globalDispatcher().addListener<InputActivityEvent>(std::bind(&InputView::onInputActivity,
                                                                             this, std::placeholders::_1),
                                                                   this);
    printf("InputView:: constructor: %p\n", this);
}

InputView::~InputView()
{
    Dispatcher::globalDispatcher().removeListener<InputActivityEvent>(this);
    Dispatcher::globalDispatcher().removeListener<InputConnectedEvent>(this);
    printf("InputView:: destructor: %p\n", this);
}

void InputView::onInputActivity(const InputActivityEvent& event)
{
    cocos2d::Node* badgeRoot = HUDHelper::createItemBox(cocos2d::Size(360.f, 100.f));
    badgeRoot->setCascadeOpacityEnabled(true);
    
    const std::string frame = event.activeController != -1 ? InputConstants::getGamepadTypeSprite(event.type) : "Keyboard_Mouse_Diagram_Simple.png";
    cocos2d::Sprite* sprite = createSprite(frame);
    cocos2d::Label* connectedLabel = createLabel("Active");
    cocos2d::Label* nameLabel = createLabel(event.name);

    nameLabel->setPosition(sprite->getPosition() + cocos2d::Vec2(sprite->getContentSize().width, 50.f));
    connectedLabel->setPosition(sprite->getPosition() + cocos2d::Vec2(sprite->getContentSize().width, 50.f - 32.f));

    badgeRoot->addChild(sprite);
    badgeRoot->addChild(nameLabel);
    badgeRoot->addChild(connectedLabel);

    ActionUtils::fadeOutAndRemove(badgeRoot, 1.f, 1.f);

    auto& children = m_root->getChildren();
    badgeRoot->setPosition(cocos2d::Vec2(400.f, (children.size() * 100.f) + 80.f));
    m_root->addChild(badgeRoot);
}

void InputView::onInputConnected(const InputConnectedEvent& event)
{
    cocos2d::Node* badgeRoot = HUDHelper::createItemBox(cocos2d::Size(360.f, 100.f));
    badgeRoot->setCascadeOpacityEnabled(true);
    
    const std::string frame = event.isConnected ? InputConstants::getGamepadTypeSprite(event.type) : "Controller_Disconnected.png";
    cocos2d::Sprite* sprite = createSprite(frame);
    cocos2d::Label* connectedLabel = createLabel(event.isConnected ? "Connected" : "Disconnected");
    cocos2d::Label* nameLabel = createLabel(event.name);

    nameLabel->setPosition(sprite->getPosition() + cocos2d::Vec2(sprite->getContentSize().width, 50.f));
    connectedLabel->setPosition(sprite->getPosition() + cocos2d::Vec2(sprite->getContentSize().width, 50.f - 32.f));

    badgeRoot->addChild(sprite);
    badgeRoot->addChild(nameLabel);
    badgeRoot->addChild(connectedLabel);

    ActionUtils::fadeOutAndRemove(badgeRoot, 1.f, 1.f);

    auto& children = m_root->getChildren();
    badgeRoot->setPosition(cocos2d::Vec2(400.f, (children.size() * 100.f) + 80.f));
    m_root->addChild(badgeRoot);
}

cocos2d::Label* InputView::createLabel(const std::string& text) const
{
    cocos2d::Label* label = HUDHelper::createLabel5x7(text, 32);;
    label->setAlignment(cocos2d::TextHAlignment::LEFT);
    label->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
    return label;
}

cocos2d::Sprite* InputView::createSprite(const std::string& frame) const
{
    cocos2d::Sprite* sprite = cocos2d::Sprite::createWithSpriteFrameName(frame);
    sprite->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
    return sprite;
}
