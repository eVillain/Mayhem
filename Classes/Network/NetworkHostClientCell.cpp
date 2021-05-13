#include "NetworkHostClientCell.h"
#include "GameViewConstants.h"
#include "HUDHelper.h"
#include "ButtonUtils.h"

USING_NS_CC;

NetworkHostClientCell::NetworkHostClientCell()
: m_hostName(nullptr)
, m_hostAddress(nullptr)
, m_hostState(nullptr)
, m_bg(nullptr)
, m_border(nullptr)
, m_readyIndicator(nullptr)
{
}

bool NetworkHostClientCell::init()
{
    if (!cocos2d::extension::TableViewCell::init())
    {
        return false;
    }
    
    setContentSize(GameViewConstants::HOST_CELL_SIZE);
    
    m_hostName = HUDHelper::createLabel3x6("Host Name:", GameViewConstants::FONT_SIZE_MEDIUM);
    m_hostName->enableOutline(cocos2d::Color4B::BLACK, 1);
    addChild(m_hostName, 1);

    m_hostAddress = HUDHelper::createLabel3x6("Host Address:", GameViewConstants::FONT_SIZE_MEDIUM);
    m_hostAddress->enableOutline(cocos2d::Color4B::BLACK, 1);
    addChild(m_hostAddress, 1);

    m_hostState = HUDHelper::createLabel3x6("State:", GameViewConstants::FONT_SIZE_MEDIUM);
    m_hostState->enableOutline(cocos2d::Color4B::BLACK, 1);
    addChild(m_hostState, 1);
    
    m_bg = HUDHelper::createHUDRect();
    m_bg->setPreferredSize(GameViewConstants::HOST_CELL_SIZE);
    m_bg->setPosition(cocos2d::Vec2(GameViewConstants::HOST_CELL_SIZE.width * 0.5f,
                                      GameViewConstants::HOST_CELL_SIZE.height * 0.5f));
    m_bg->setColor(GameViewConstants::TABLEVIEW_CELL_COLOR);
    addChild(m_bg, -1);
    
    m_border = HUDHelper::createHUDOutline();
    m_border->setPreferredSize(GameViewConstants::HOST_CELL_SIZE - cocos2d::Size(4,4));
    m_border->setPosition(cocos2d::Vec2(GameViewConstants::HOST_CELL_SIZE.width * 0.5f,
                                        GameViewConstants::HOST_CELL_SIZE.height * 0.5f));
    addChild(m_border);
    
    m_readyIndicator = HUDHelper::createHUDRect();
    m_readyIndicator->setPreferredSize(cocos2d::Size(16, 16));
    m_readyIndicator->setPosition(cocos2d::Vec2(GameViewConstants::HOST_CELL_SIZE.width - 32.f,
                                                GameViewConstants::HOST_CELL_SIZE.height * 0.5f));
    m_readyIndicator->setColor(cocos2d::Color3B::GRAY);
    addChild(m_readyIndicator);
    
    return true;
}

void NetworkHostClientCell::refreshPositions()
{
    const cocos2d::Vec2 center = GameViewConstants::HOST_CELL_SIZE * 0.5f;
    const float PADDING = 8.f;
    const cocos2d::Vec2 HOST_NAME_POS = cocos2d::Vec2(PADDING + m_hostName->getContentSize().width * 0.5f,
                                                      center.y + m_hostName->getContentSize().height * 0.6f);
    const cocos2d::Vec2 HOST_ADDRESS_POS = cocos2d::Vec2(PADDING + m_hostAddress->getContentSize().width * 0.5f,
                                                         center.y);
    const cocos2d::Vec2 HOST_STATE_POS = cocos2d::Vec2(PADDING + m_hostState->getContentSize().width * 0.5f,
                                                         center.y - m_hostState->getContentSize().height * 0.6f);

    m_hostName->setPosition(HOST_NAME_POS);
    m_hostAddress->setPosition(HOST_ADDRESS_POS);
    m_hostState->setPosition(HOST_STATE_POS);
}
