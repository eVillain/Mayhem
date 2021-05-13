#ifndef NETWORK_HOST_GAME_LEVEL_CELL_H
#define NETWORK_HOST_GAME_LEVEL_CELL_H

#include "cocos2d.h"
#include <extensions/GUI/CCScrollView/CCTableView.h>

class NetworkHostGameLevelCell : public cocos2d::extension::TableViewCell
{
public:
    NetworkHostGameLevelCell();
    
    virtual bool init();
    
    void refreshPositions();
    
    CREATE_FUNC(NetworkHostGameLevelCell);

    cocos2d::Label* getGameLevelLabel() const { return m_gameLevel; }
    cocos2d::ui::Scale9Sprite* getSelectedIndicator() const { return m_selectedIndicator; }

private:
    cocos2d::Label* m_gameLevel;
    
    cocos2d::ui::Scale9Sprite* m_bg;
    cocos2d::ui::Scale9Sprite* m_border;
    cocos2d::ui::Scale9Sprite* m_selectedIndicator;
};

#endif // NETWORK_HOST_GAME_LEVEL_CELL_H
