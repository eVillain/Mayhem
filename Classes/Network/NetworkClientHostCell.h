#ifndef NETWORK_CLIENT_HOST_CELL_H
#define NETWORK_CLIENT_HOST_CELL_H

#include "cocos2d.h"
#include <extensions/GUI/CCScrollView/CCTableView.h>

class NetworkClientHostCell : public cocos2d::extension::TableViewCell
{
public:
    NetworkClientHostCell();
    
    virtual bool init();
    
    void refreshPositions();
    
    CREATE_FUNC(NetworkClientHostCell);

    cocos2d::Label* getHostNameLabel() { return m_hostName; }
    cocos2d::Label* getHostAddressLabel() { return m_hostAddress; }

private:
    cocos2d::Label* m_hostName;
    cocos2d::Label* m_hostAddress;
};

#endif // NETWORK_CLIENT_HOST_CELL_H
