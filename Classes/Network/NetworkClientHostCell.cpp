#include "NetworkClientHostCell.h"
#include "GameViewConstants.h"
#include "HUDHelper.h"

USING_NS_CC;

NetworkClientHostCell::NetworkClientHostCell()
: m_hostName(nullptr)
, m_hostAddress(nullptr)
{
}

bool NetworkClientHostCell::init()
{
    if (!cocos2d::extension::TableViewCell::init())
    {
        return false;
    }
    
    setContentSize(GameViewConstants::CLIENT_CELL_SIZE);

    m_hostName = HUDHelper::createLabel3x6("Host Name", GameViewConstants::FONT_SIZE_MEDIUM);
    m_hostName->enableOutline(cocos2d::Color4B::BLACK, 1);
    addChild(m_hostName, 1);

    m_hostAddress = HUDHelper::createLabel3x6("Host Address", GameViewConstants::FONT_SIZE_MEDIUM);
    m_hostAddress->enableOutline(cocos2d::Color4B::BLACK, 1);
    addChild(m_hostAddress, 1);
    
    cocos2d::ui::Scale9Sprite* sprite = HUDHelper::createHUDRect();
    sprite->setPreferredSize(GameViewConstants::CLIENT_CELL_SIZE);
    sprite->setPosition(cocos2d::Vec2(GameViewConstants::CLIENT_CELL_SIZE.width * 0.5f,
                                      GameViewConstants::CLIENT_CELL_SIZE.height * 0.5f));
    sprite->setColor(GameViewConstants::TABLEVIEW_CELL_COLOR);
    addChild(sprite, -1);
    
    return true;
}

void NetworkClientHostCell::refreshPositions()
{
    const float PADDING = 8.f;
    const cocos2d::Vec2 HOST_NAME_POS = cocos2d::Vec2(PADDING + m_hostName->getContentSize().width * 0.5f,
                                                      + PADDING + m_hostName->getContentSize().height * 0.5f);
    const cocos2d::Vec2 HOST_ADDRESS_POS = cocos2d::Vec2(PADDING + m_hostAddress->getContentSize().width * 0.5f,
                                                         HOST_NAME_POS.y + m_hostAddress->getContentSize().height);

    m_hostName->setPosition(HOST_NAME_POS);
    m_hostAddress->setPosition(HOST_ADDRESS_POS);
}
