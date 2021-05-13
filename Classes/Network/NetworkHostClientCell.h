#ifndef NETWORK_HOST_CLIENT_CELL_H
#define NETWORK_HOST_CLIENT_CELL_H

#include "cocos2d.h"
#include <extensions/GUI/CCScrollView/CCTableView.h>

class NetworkHostClientCell : public cocos2d::extension::TableViewCell
{
public:
    NetworkHostClientCell();
    
    virtual bool init();
    
    void refreshPositions();
    
    CREATE_FUNC(NetworkHostClientCell);

    cocos2d::Label* getHostNameLabel() const { return m_hostName; }
    cocos2d::Label* getHostAddressLabel() const { return m_hostAddress; }
    cocos2d::Label* getHostStateLabel() const { return m_hostState; }
    cocos2d::ui::Scale9Sprite* getReadyIndicator() const { return m_readyIndicator; }

private:
    cocos2d::Label* m_hostName;
    cocos2d::Label* m_hostAddress;
    cocos2d::Label* m_hostState;

    cocos2d::ui::Scale9Sprite* m_bg;
    cocos2d::ui::Scale9Sprite* m_border;
    cocos2d::ui::Scale9Sprite* m_readyIndicator;
};

#endif // NETWORK_HOST_CLIENT_CELL_H
