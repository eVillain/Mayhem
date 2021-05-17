#include "MainMenuView.h"
#include "ButtonUtils.h"
#include "GameViewConstants.h"
#include "HUDHelper.h"
#include "ActionUtils.h"

USING_NS_CC;

// on "init" you need to initialize your instance
bool MainMenuView::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    m_title = HUDHelper::createLabel5x7("Mayhem Royale v0.0 - Main Menu", 24);
    m_playerName = HUDHelper::createLabel5x7("PlayerNameGoesHere", 20);

    // position the label on the center of the screen
    m_title->setPosition(Vec2(origin.x + visibleSize.width/2,
                              origin.y + visibleSize.height - m_title->getContentSize().height));
    m_playerName->setPosition(m_title->getPosition() - Vec2(0.f, m_playerName->getContentSize().height));

    // add the label as a child to this layer
    addChild(m_title);
    addChild(m_playerName);

    cocos2d::SpriteFrameCache* cache = cocos2d::SpriteFrameCache::getInstance();
    cache->addSpriteFramesWithFile(GameViewConstants::HUD_SPRITE_BATCH_FILE);
    
    const float PADDING = 8.f;
    float buttonPosY = (GameViewConstants::BUTTON_SIZE_MENU.height + PADDING) * 2.f;
    
    m_startGameButton = ButtonUtils::createButton(cocos2d::Vec2(0, buttonPosY) + visibleSize*0.5f,
                                                  GameViewConstants::BUTTON_SIZE_MENU,
                                                  "Debug Local",
                                                  GameViewConstants::FONT_SIZE_LARGE);
    buttonPosY -= (GameViewConstants::BUTTON_SIZE_MENU.height + PADDING);
    m_hostGameButton = ButtonUtils::createButton(cocos2d::Vec2(0, buttonPosY) + visibleSize*0.5f,
                                                 GameViewConstants::BUTTON_SIZE_MENU,
                                                 "Host Game",
                                                 GameViewConstants::FONT_SIZE_LARGE);
    buttonPosY -= (GameViewConstants::BUTTON_SIZE_MENU.height + PADDING);
    m_joinGameButton = ButtonUtils::createButton(cocos2d::Vec2(0, buttonPosY) + visibleSize*0.5f,
                                                 GameViewConstants::BUTTON_SIZE_MENU,
                                                 "Join Game",
                                                 GameViewConstants::FONT_SIZE_LARGE);
    buttonPosY -= (GameViewConstants::BUTTON_SIZE_MENU.height + PADDING);
    m_settingsGameButton = ButtonUtils::createButton(cocos2d::Vec2(0, buttonPosY) + visibleSize*0.5f,
                                                 GameViewConstants::BUTTON_SIZE_MENU,
                                                 "Settings",
                                                 GameViewConstants::FONT_SIZE_LARGE);
    buttonPosY -= (GameViewConstants::BUTTON_SIZE_MENU.height + PADDING);
    m_replayEditorButton = ButtonUtils::createButton(cocos2d::Vec2(0, buttonPosY) + visibleSize*0.5f,
                                                 GameViewConstants::BUTTON_SIZE_MENU,
                                                 "Replay Editor",
                                                 GameViewConstants::FONT_SIZE_LARGE);
    buttonPosY -= (GameViewConstants::BUTTON_SIZE_MENU.height + PADDING);
    m_exitGameButton = ButtonUtils::createButton(cocos2d::Vec2(0, buttonPosY) + visibleSize*0.5f,
                                                 GameViewConstants::BUTTON_SIZE_MENU,
                                                 "Exit Game",
                                                 GameViewConstants::FONT_SIZE_LARGE);
    
    addChild(m_startGameButton);
    addChild(m_hostGameButton);
    addChild(m_joinGameButton);
    addChild(m_settingsGameButton);
    addChild(m_replayEditorButton);
    addChild(m_exitGameButton);

    const std::vector<cocos2d::Node*> nodes = {
        m_title,
        m_startGameButton,
        m_hostGameButton,
        m_joinGameButton,
        m_settingsGameButton,
        m_replayEditorButton,
        m_exitGameButton,
    };
    
    ActionUtils::fadeInElements(nodes, 0.1f, 0.05f);
    
    return true;
}

