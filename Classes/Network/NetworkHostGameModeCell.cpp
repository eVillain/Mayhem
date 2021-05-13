#include "NetworkHostGameModeCell.h"
#include "GameViewConstants.h"
#include "HUDHelper.h"
#include "ButtonUtils.h"

USING_NS_CC;

NetworkHostGameModeCell::NetworkHostGameModeCell()
: m_gameMode(nullptr)
, m_bg(nullptr)
, m_border(nullptr)
, m_selectedIndicator(nullptr)
{
}

bool NetworkHostGameModeCell::init()
{
    if (!cocos2d::extension::TableViewCell::init())
    {
        return false;
    }
    
    setContentSize(GameViewConstants::GAMEMODE_CELL_SIZE);
    
    m_gameMode = HUDHelper::createLabel3x6("Mode", GameViewConstants::FONT_SIZE_MEDIUM);
    m_gameMode->enableOutline(cocos2d::Color4B::BLACK, 1);
    addChild(m_gameMode, 1);
    
    m_bg = HUDHelper::createHUDRect();
    m_bg->setPreferredSize(GameViewConstants::GAMEMODE_CELL_SIZE);
    m_bg->setPosition(cocos2d::Vec2(GameViewConstants::GAMEMODE_CELL_SIZE.width * 0.5f,
                                      GameViewConstants::GAMEMODE_CELL_SIZE.height * 0.5f));
    m_bg->setColor(GameViewConstants::TABLEVIEW_CELL_COLOR);
    addChild(m_bg, -1);
    
    m_border = HUDHelper::createHUDOutline();
    m_border->setPreferredSize(GameViewConstants::GAMEMODE_CELL_SIZE - cocos2d::Size(4,4));
    m_border->setPosition(cocos2d::Vec2(GameViewConstants::GAMEMODE_CELL_SIZE.width * 0.5f,
                                        GameViewConstants::GAMEMODE_CELL_SIZE.height * 0.5f));
    addChild(m_border);
    
    m_selectedIndicator = HUDHelper::createHUDRect();
    m_selectedIndicator->setPreferredSize(cocos2d::Size(16, 16));
    m_selectedIndicator->setPosition(cocos2d::Vec2(GameViewConstants::GAMEMODE_CELL_SIZE.width - 32.f,
                                                   GameViewConstants::GAMEMODE_CELL_SIZE.height * 0.5f));
    m_selectedIndicator->setColor(cocos2d::Color3B::GRAY);
    addChild(m_selectedIndicator);
    
    return true;
}

void NetworkHostGameModeCell::refreshPositions()
{
    const cocos2d::Vec2 center = GameViewConstants::GAMEMODE_CELL_SIZE * 0.5f;
    const float PADDING = 8.f;
    const cocos2d::Vec2 GAME_MODE_POS = cocos2d::Vec2(PADDING + m_gameMode->getContentSize().width * 0.5f,
                                                      center.y);
    
    m_gameMode->setPosition(GAME_MODE_POS);
}
