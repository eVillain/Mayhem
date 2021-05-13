#include "GameTableView.h"
#include "GameViewConstants.h"
#include "HUDHelper.h"

USING_NS_CC;

GameTableView::GameTableView()
{
}

void GameTableView::configure(const cocos2d::Size& size,
                              const cocos2d::Vec2& position,
                              cocos2d::extension::TableViewDataSource* dataSource,
                              cocos2d::extension::TableViewDelegate* delegate,
                              const std::string header /* = "" */)
{
    setContentSize(size);
    setPosition(position);
    
    bool addHeader = header.length() > 0;
    const cocos2d::Size TABLE_HEADER_SIZE = cocos2d::Size(size.width, 30.f);
    const cocos2d::Size TABLE_SIZE = cocos2d::Size(size.width, size.height - (addHeader ? TABLE_HEADER_SIZE.height : 0.f));

    m_tableView = cocos2d::extension::TableView::create(dataSource, TABLE_SIZE);
    m_tableView->setDelegate(delegate);
    addChild(m_tableView);
    
    m_tableBG = HUDHelper::createHUDFill();
    m_tableBG->setColor(GameViewConstants::TABLEVIEW_BG_COLOR);
    m_tableBG->setOpacity(63);
    m_tableBG->setPreferredSize(TABLE_SIZE + cocos2d::Size(2,2));
    m_tableBG->setPosition(m_tableView->getPosition());
    m_tableBG->setAnchorPoint(cocos2d::Vec2::ZERO);
    addChild(m_tableBG);

    if (addHeader)
    {
        const cocos2d::Vec2 TABLE_HEADER_POSITION = cocos2d::Vec2(size.width * 0.5f,
                                                                  size.height - TABLE_HEADER_SIZE.height * 0.5f);
        m_tableHeaderBG = HUDHelper::createHUDFill();
        m_tableHeaderBG->setPreferredSize(TABLE_HEADER_SIZE);
        m_tableHeaderBG->setColor(GameViewConstants::TABLEVIEW_HEADER_BG_COLOR);
        m_tableHeaderBG->setOpacity(63);
        m_tableHeaderBG->setPosition(TABLE_HEADER_POSITION);
        addChild(m_tableHeaderBG);
        m_tableHeaderBorder = HUDHelper::createHUDOutline();
        m_tableHeaderBorder->setPreferredSize(TABLE_HEADER_SIZE - cocos2d::Size(2,2));
        m_tableHeaderBorder->setColor(GameViewConstants::BUTTON_BORDER_COLOR);
        m_tableHeaderBorder->setPosition(TABLE_HEADER_POSITION);
        addChild(m_tableHeaderBorder);
        m_tableHeaderLabel = HUDHelper::createLabel3x6(header, 24);
        m_tableHeaderLabel->setPosition(TABLE_HEADER_POSITION);
        addChild(m_tableHeaderLabel);
    }
}
