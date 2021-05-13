#ifndef SettingsView_h
#define SettingsView_h

#include "cocos2d.h"
#include "cocos-ext.h"

class GameSettings;

class SettingsView
: public cocos2d::extension::TableViewDataSource
, public cocos2d::extension::TableViewDelegate
{
public:
    SettingsView(std::shared_ptr<GameSettings> gameSettings);
    ~SettingsView();
    
    void initialize();
    
    const cocos2d::RefPtr<cocos2d::ui::Button>& getExitButton() const { return m_exitButton; }
    const cocos2d::RefPtr<cocos2d::LayerColor>& getLayer() const { return m_layer; }
    
    // TableViewDataSource
    cocos2d::Size tableCellSizeForIndex(cocos2d::extension::TableView* table, ssize_t idx) override;
    cocos2d::Size cellSizeForTable(cocos2d::extension::TableView* table) override;
    cocos2d::extension::TableViewCell* tableCellAtIndex(cocos2d::extension::TableView *table, ssize_t idx) override;
    ssize_t numberOfCellsInTableView(cocos2d::extension::TableView *table) override;
    // TableViewDelegate
    void tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell) override;
    
private:
    std::shared_ptr<GameSettings> m_gameSettings;
    
    cocos2d::RefPtr<cocos2d::ui::Button> m_exitButton;
    cocos2d::RefPtr<cocos2d::LayerColor> m_layer;
    cocos2d::RefPtr<cocos2d::Label> m_settingsTitleLabel;
    cocos2d::RefPtr<cocos2d::extension::TableView> m_settingsTable;
    cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> m_settingsTableBG;
        
    void onSettingToggle(const ssize_t idx);
};

#endif /* SettingsView_h */
