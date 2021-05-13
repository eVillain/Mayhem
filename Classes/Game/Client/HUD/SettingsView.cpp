#include "SettingsView.h"
#include "SettingCell.h"
#include "HUDHelper.h"
#include "GameSettings.h"
#include "GameViewConstants.h"
#include "ButtonUtils.h"

SettingsView::SettingsView(std::shared_ptr<GameSettings> gameSettings)
: m_gameSettings(gameSettings)
, m_exitButton(nullptr)
, m_layer(nullptr)
, m_settingsTitleLabel(nullptr)
, m_settingsTable(nullptr)
, m_settingsTableBG(nullptr)
{
}

SettingsView::~SettingsView()
{
}

void SettingsView::initialize()
{
    const cocos2d::Size winSize = cocos2d::Director::getInstance()->getWinSize();

    const cocos2d::Vec2 EXIT_BUTTON_POSITION = cocos2d::Vec2(GameViewConstants::BUTTON_SIZE_EXIT.width * 0.5,
                                                             winSize.height - (GameViewConstants::BUTTON_SIZE_EXIT.height * 0.5));

    m_layer = cocos2d::LayerColor::create(cocos2d::Color4B(0,0,0,127));
    
    m_exitButton = ButtonUtils::createButton(EXIT_BUTTON_POSITION,
                                             GameViewConstants::BUTTON_SIZE_EXIT,
                                             "Back",
                                             GameViewConstants::FONT_SIZE_MEDIUM);
    m_layer->addChild(m_exitButton);
    
    m_settingsTitleLabel = HUDHelper::createLabel5x7("Settings", GameViewConstants::FONT_SIZE_LARGE);
    m_settingsTitleLabel->setPosition(cocos2d::Vec2(winSize.width * 0.5f, winSize.height - 40.f));
    m_layer->addChild(m_settingsTitleLabel);
    
    const cocos2d::Size TABLE_SIZE = cocos2d::Size(GameViewConstants::SETTING_CELL_SIZE.width, winSize.height - 100);
    m_settingsTable = cocos2d::extension::TableView::create(this, TABLE_SIZE);
    m_settingsTable->setPosition(cocos2d::Vec2(100, 0));
    m_settingsTable->setDelegate(this);
    
    m_settingsTableBG = HUDHelper::createHUDFill();
    m_settingsTableBG->setColor(cocos2d::Color3B::WHITE);
    m_settingsTableBG->setOpacity(63);
    m_settingsTableBG->setPreferredSize(TABLE_SIZE + cocos2d::Size(2,2));
    m_settingsTableBG->setPosition(m_settingsTable->getPosition());
    m_settingsTableBG->setAnchorPoint(cocos2d::Vec2::ZERO);
    m_layer->addChild(m_settingsTableBG);
    
    m_layer->addChild(m_settingsTable);
}

cocos2d::Size SettingsView::tableCellSizeForIndex(cocos2d::extension::TableView* table, ssize_t idx)
{
    return GameViewConstants::SETTING_CELL_SIZE;
}

cocos2d::Size SettingsView::cellSizeForTable(cocos2d::extension::TableView* table)
{
    return GameViewConstants::SETTING_CELL_SIZE;
}

cocos2d::extension::TableViewCell* SettingsView::tableCellAtIndex(cocos2d::extension::TableView* table, ssize_t idx)
{
    SettingCell* cell = static_cast<SettingCell*>(table->dequeueCell());
    if (!cell)
    {
        cell = SettingCell::create();
    }
    
    auto& values = m_gameSettings->getValues();
    if (idx <= values.size())
    {
        auto it = values.begin();
        std::advance(it, idx);
        const auto& value = it->second;
        
        cell->setup(it->first, value);
        if (cell->getToggleButton())
        {
            cell->getToggleButton()->addClickEventListener(std::bind(&SettingsView::onSettingToggle, this, idx));
        }
    }
    
    return cell;
}

ssize_t SettingsView::numberOfCellsInTableView(cocos2d::extension::TableView* table)
{
    return m_gameSettings->getValues().size();
}

void SettingsView::tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell)
{

}

void SettingsView::onSettingToggle(const ssize_t idx)
{
    auto& values = m_gameSettings->getValues();
    if (idx <= values.size())
    {
        auto it = values.begin();
        std::advance(it, idx);
        auto& value = it->second;
        if (value.getType() != cocos2d::Value::Type::BOOLEAN)
        {
            return;
        }
        auto newVal = cocos2d::Value(!value.asBool());
        m_gameSettings->setValue(it->first, newVal);
        
        SettingCell* cell = static_cast<SettingCell*>(m_settingsTable->cellAtIndex(idx));
        cell->setup(it->first, newVal);
    }
}
