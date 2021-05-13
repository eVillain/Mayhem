#include "NetworkView.h"
#include "ButtonUtils.h"
#include "GameViewConstants.h"
#include "HUDHelper.h"
#include "NetworkChatView.h"

USING_NS_CC;

bool NetworkView::init()
{
    if (!Layer::init())
    {
        return false;
    }
    
    setupDefaultButtons();
    
    return true;
}

NetworkView::NetworkView(const NetworkView::Mode mode)
: m_mode(mode)
, m_title(nullptr)
, m_readyButton(nullptr)
, m_exitButton(nullptr)
, m_chatView(nullptr)
{
}

void NetworkView::setupDefaultButtons()
{
    const cocos2d::Size winSize = cocos2d::Director::getInstance()->getWinSize();

    const cocos2d::Vec2 EXIT_BUTTON_POSITION = cocos2d::Vec2(GameViewConstants::BUTTON_SIZE_EXIT.width * 0.5,
                                                             winSize.height - (GameViewConstants::BUTTON_SIZE_EXIT.height * 0.5));
    
    const cocos2d::Vec2 READY_BUTTON_POSITION = cocos2d::Vec2(winSize.width - GameViewConstants::BUTTON_SIZE_READY.width * 0.5f,
                                                              GameViewConstants::BUTTON_SIZE_READY.height * 0.5);
    
    m_exitButton = ButtonUtils::createButton(EXIT_BUTTON_POSITION,
                                             GameViewConstants::BUTTON_SIZE_EXIT,
                                             "Back To Main Menu",
                                             GameViewConstants::FONT_SIZE_MEDIUM);
    
  
    m_readyButton = ButtonUtils::createButton(READY_BUTTON_POSITION,
                                              GameViewConstants::BUTTON_SIZE_READY,
                                              "Ready",
                                              GameViewConstants::FONT_SIZE_MEDIUM);
}

void NetworkView::showReadyButton(const bool show)
{
    const bool hasParent = m_readyButton->getParent() != nullptr;
    if (show && !hasParent)
    {
        addChild(m_readyButton);
    }
    else if (!show && hasParent)
    {
        m_readyButton->removeFromParent();
    }
}

void NetworkView::showExitButton(const bool show)
{
    const bool hasParent = m_exitButton->getParent() != nullptr;
    if (show && !hasParent)
    {
        addChild(m_exitButton);
    }
    else if (!show && hasParent)
    {
        m_exitButton->removeFromParent();
    }
}

void NetworkView::setupChatView()
{
    if (!m_chatView)
    {
        m_chatView = NetworkChatView::create();
        addChild(m_chatView);
    }
}

void NetworkView::removeChatView()
{
    if (m_chatView)
    {
        removeChild(m_chatView);
        m_chatView = nullptr;
    }
}
