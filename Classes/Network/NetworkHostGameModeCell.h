#ifndef NETWORK_HOST_GAME_MODE_CELL_H
#define NETWORK_HOST_GAME_MODE_CELL_H

#include "cocos2d.h"
#include <extensions/GUI/CCScrollView/CCTableView.h>

class NetworkHostGameModeCell : public cocos2d::extension::TableViewCell
{
public:
    NetworkHostGameModeCell();
    
    virtual bool init();
    
    void refreshPositions();
    
    CREATE_FUNC(NetworkHostGameModeCell);

    cocos2d::Label* getGameModeLabel() const { return m_gameMode; }
    cocos2d::ui::Scale9Sprite* getSelectedIndicator() const { return m_selectedIndicator; }

private:
    cocos2d::Label* m_gameMode;
    
    cocos2d::ui::Scale9Sprite* m_bg;
    cocos2d::ui::Scale9Sprite* m_border;
    cocos2d::ui::Scale9Sprite* m_selectedIndicator;
};

#endif // NETWORK_HOST_GAME_MODE_CELL_H
