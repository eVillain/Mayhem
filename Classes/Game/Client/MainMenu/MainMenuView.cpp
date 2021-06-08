#include "MainMenuView.h"

#include "ButtonUtils.h"
#include "GameViewConstants.h"
#include "HUDHelper.h"
#include "ActionUtils.h"
#include "HUDConstants.h"
#include "EntityView.h"
#include "InputView.h"

USING_NS_CC;

MainMenuView::MainMenuView()
{
    printf("MainMenuView:: constructor: %p\n", this);
}

MainMenuView::~MainMenuView()
{
    printf("MainMenuView:: destructor: %p\n", this);
}

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

    m_title->setPosition(Vec2(origin.x + visibleSize.width/2,
                              origin.y + visibleSize.height - m_title->getContentSize().height));
    addChild(m_title);
    addChild(m_playerName);

    cocos2d::SpriteFrameCache* cache = cocos2d::SpriteFrameCache::getInstance();
    cache->addSpriteFramesWithFile(HUDConstants::HUD_SPRITE_BATCH_FILE);
    cache->addSpriteFramesWithFile(GameViewConstants::SPRITE_BATCH_FILE);

    const float PADDING = 8.f;
    const float buttonPosX = visibleSize.width * 0.33f;
    float buttonPosY = (visibleSize.height * 0.5f) + (GameViewConstants::BUTTON_SIZE_MENU.height + PADDING) * 2.f;
    
    m_startGameButton = ButtonUtils::createButton(cocos2d::Vec2(buttonPosX, buttonPosY),
                                                  GameViewConstants::BUTTON_SIZE_MENU,
                                                  "Debug Local",
                                                  GameViewConstants::FONT_SIZE_LARGE);
    buttonPosY -= (GameViewConstants::BUTTON_SIZE_MENU.height + PADDING);
    m_hostGameButton = ButtonUtils::createButton(cocos2d::Vec2(buttonPosX, buttonPosY),
                                                 GameViewConstants::BUTTON_SIZE_MENU,
                                                 "Host Game",
                                                 GameViewConstants::FONT_SIZE_LARGE);
    buttonPosY -= (GameViewConstants::BUTTON_SIZE_MENU.height + PADDING);
    m_joinGameButton = ButtonUtils::createButton(cocos2d::Vec2(buttonPosX, buttonPosY),
                                                 GameViewConstants::BUTTON_SIZE_MENU,
                                                 "Join Game",
                                                 GameViewConstants::FONT_SIZE_LARGE);
    buttonPosY -= (GameViewConstants::BUTTON_SIZE_MENU.height + PADDING);
    m_settingsGameButton = ButtonUtils::createButton(cocos2d::Vec2(buttonPosX, buttonPosY),
                                                 GameViewConstants::BUTTON_SIZE_MENU,
                                                 "Settings",
                                                 GameViewConstants::FONT_SIZE_LARGE);
    buttonPosY -= (GameViewConstants::BUTTON_SIZE_MENU.height + PADDING);
    m_replayEditorButton = ButtonUtils::createButton(cocos2d::Vec2(buttonPosX, buttonPosY),
                                                 GameViewConstants::BUTTON_SIZE_MENU,
                                                 "Replay Editor",
                                                 GameViewConstants::FONT_SIZE_LARGE);
    buttonPosY -= (GameViewConstants::BUTTON_SIZE_MENU.height + PADDING);
    m_exitGameButton = ButtonUtils::createButton(cocos2d::Vec2(buttonPosX, buttonPosY),
                                                 GameViewConstants::BUTTON_SIZE_MENU,
                                                 "Exit Game",
                                                 GameViewConstants::FONT_SIZE_LARGE);
    
    addChild(m_startGameButton);
    addChild(m_hostGameButton);
    addChild(m_joinGameButton);
    addChild(m_settingsGameButton);
    addChild(m_replayEditorButton);
    addChild(m_exitGameButton);

    m_playerView = std::make_shared<EntityView>(0, EntityType::PlayerEntity);
    m_playerView->setupAnimations(CharacterType::Character_Base);
    m_playerView->runAnimation(AnimationState::Idle);
    auto headSprite = m_playerView->getSecondarySprites().at(EntityView::HEAD_INDEX);
    headSprite->runAction(m_playerView->getAnimations().at(AnimationState::IdleFace));
    m_playerView->getSprite()->getTexture()->setAliasTexParameters();
    m_playerView->setFlippedX(true);
    
    auto leftArm = m_playerView->getSecondarySprites().at(EntityView::ARM_LEFT_INDEX);
    leftArm->setAnchorPoint(EntityView::ARM_L_ANCHOR_FLIPPED);
    leftArm->setPosition(EntityView::ARM_L_POS_FLIPPED);
    
    m_playerNode = cocos2d::Node::create();
    m_playerNode->setPosition(cocos2d::Vec2(visibleSize.width * 0.66f, visibleSize.height * 0.5f));
    m_playerNode->setScale(8.f);
    m_playerNode->addChild(m_playerView->getSprite());

    addChild(m_playerNode);
    
    m_playerName->setPosition(m_playerNode->getPosition() - Vec2(0.f, 80.f));

    const std::vector<cocos2d::Node*> nodes = {
        m_title,
        m_startGameButton,
        m_hostGameButton,
        m_joinGameButton,
        m_settingsGameButton,
        m_replayEditorButton,
        m_exitGameButton,
        m_playerName,
        m_playerView->getSprite(),
    };
    
    ActionUtils::fadeInElements(nodes, 0.1f, 0.05f);
    
    m_inputView = std::make_shared<InputView>();
    addChild(m_inputView->getRoot(), 100);
    
    return true;
}

