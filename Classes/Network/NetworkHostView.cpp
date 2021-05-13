#include "NetworkHostView.h"
#include "NetworkHostViewController.h"
#include "SimpleAudioEngine.h"
#include "GameViewConstants.h"
#include "HUDHelper.h"

USING_NS_CC;

NetworkHostView::NetworkHostView()
: NetworkView(NetworkView::Mode::HOST)
, m_clientTableView(nullptr)
, m_gameTypeTableView(nullptr)
, m_gameLevelTableView(nullptr)
{
}

bool NetworkHostView::init()
{
    if (!NetworkView::init())
    {
        return false;
    }

    return true;
}

void NetworkHostView::setupHostUI(NetworkHostViewController* controller)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    m_title = HUDHelper::createLabel5x7("Network Host", GameViewConstants::FONT_SIZE_LARGE);
    m_title->setPosition(Vec2(origin.x + visibleSize.width/2,
                              origin.y + visibleSize.height - m_title->getContentSize().height));
    addChild(m_title, 1);
    
    const cocos2d::Size TABLE_SIZE = cocos2d::Size(GameViewConstants::CLIENT_CELL_SIZE.width,
                                                   visibleSize.height - m_exitButton->getContentSize().height);
    const cocos2d::Vec2 TABLE_POSITION = cocos2d::Vec2(visibleSize.width - TABLE_SIZE.width, 0);
    const cocos2d::Size GAMELEVEL_TABLE_SIZE = cocos2d::Size(GameViewConstants::CLIENT_CELL_SIZE.width, 120.f);
    const cocos2d::Vec2 GAMELEVEL_TABLE_POSITION = cocos2d::Vec2(0, 0.f);
    const cocos2d::Size GAMETYPE_TABLE_SIZE = cocos2d::Size(GameViewConstants::CLIENT_CELL_SIZE.width, 120.f);
    const cocos2d::Vec2 GAMETYPE_TABLE_POSITION = cocos2d::Vec2(0, GAMELEVEL_TABLE_SIZE.height);

    m_clientTableView = GameTableView::create();
    m_gameTypeTableView = GameTableView::create();
    m_gameLevelTableView = GameTableView::create();

    m_clientTableView->configure(TABLE_SIZE,
                                 TABLE_POSITION,
                                 controller,
                                 controller,
                                 "Connected Clients");
    m_gameTypeTableView->configure(GAMETYPE_TABLE_SIZE,
                                   GAMETYPE_TABLE_POSITION,
                                   controller,
                                   controller,
                                   "Game Mode");
    m_gameLevelTableView->configure(GAMELEVEL_TABLE_SIZE,
                                    GAMELEVEL_TABLE_POSITION,
                                    controller,
                                    controller,
                                    "Level");
    
    addChild(m_clientTableView);
    addChild(m_gameTypeTableView);
    addChild(m_gameLevelTableView);
    
    showExitButton(true);
}
