#include "NetworkClientView.h"

#include "NetworkClientViewController.h"
#include "ButtonUtils.h"
#include "GameViewConstants.h"
#include "HUDHelper.h"

USING_NS_CC;

NetworkClientView::NetworkClientView()
: NetworkView(NetworkView::Mode::CLIENT)
, m_hostTableView(nullptr)
{
}

bool NetworkClientView::init()
{
    if (!NetworkView::init())
    {
        return false;
    }
    
    return true;
}

void NetworkClientView::setupClientUI(NetworkClientViewController* controller)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    m_title = HUDHelper::createLabel5x7("Network Client", GameViewConstants::FONT_SIZE_LARGE);
    m_title->setPosition(Vec2(origin.x + visibleSize.width/2,
                              origin.y + visibleSize.height - m_title->getContentSize().height));
    addChild(m_title, 1);

    const cocos2d::Size TABLE_SIZE = cocos2d::Size(GameViewConstants::CLIENT_CELL_SIZE.width,
                                                   visibleSize.height - m_exitButton->getContentSize().height);
    const cocos2d::Vec2 TABLE_POSITION = cocos2d::Vec2(visibleSize.width - TABLE_SIZE.width, 0);

    m_hostTableView = GameTableView::create();
    m_hostTableView->configure(TABLE_SIZE,
                               TABLE_POSITION,
                               controller,
                               controller,
                               "Available Hosts");
    addChild(m_hostTableView);
    
    showExitButton(true);
}

void NetworkClientView::setHostTableVisible(const bool visible)
{
    if (!visible && m_hostTableView->getParent())
    {
        m_hostTableView->removeFromParent();
    }
    else if (visible && !m_hostTableView->getParent())
    {
        addChild(m_hostTableView);
    }
}
