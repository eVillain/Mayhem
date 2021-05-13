#ifndef GAME_TABLE_VIEW_H
#define GAME_TABLE_VIEW_H

#include "cocos2d.h"
#include "cocos-ext.h"

class GameTableView : public cocos2d::Node
{
public:
    GameTableView();
            
    CREATE_FUNC(GameTableView);

    void configure(const cocos2d::Size& size,
                   const cocos2d::Vec2& position,
                   cocos2d::extension::TableViewDataSource* dataSource,
                   cocos2d::extension::TableViewDelegate* delegate,
                   const std::string header = "");
    
    const cocos2d::RefPtr<cocos2d::extension::TableView>& getTable() const { return m_tableView; }
    const cocos2d::RefPtr<cocos2d::ui::Scale9Sprite>& getTableBG() const { return m_tableBG; }
    const cocos2d::RefPtr<cocos2d::ui::Scale9Sprite>& getHeaderBG() const { return m_tableHeaderBG; }
    const cocos2d::RefPtr<cocos2d::ui::Scale9Sprite>& getHeaderBorder() const { return m_tableHeaderBorder; }
    const cocos2d::RefPtr<cocos2d::Label>& getHeadeLabel() const { return m_tableHeaderLabel; }
    
private:
    cocos2d::RefPtr<cocos2d::extension::TableView> m_tableView;
    cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> m_tableBG;
    
    cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> m_tableHeaderBG;
    cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> m_tableHeaderBorder;
    cocos2d::RefPtr<cocos2d::Label> m_tableHeaderLabel;
};

#endif // GAME_TABLE_VIEW_H
