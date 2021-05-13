#ifndef NETWORK_CLIENT_VIEW_H
#define NETWORK_CLIENT_VIEW_H

#include "NetworkView.h"
#include "GameTableView.h"
#include "cocos-ext.h"

class NetworkClientViewController;

class NetworkClientView : public NetworkView
{
public:
    NetworkClientView();
    
    virtual bool init();
    
    void setupClientUI(NetworkClientViewController* controller);
    void setHostTableVisible(const bool visible);
    
    CREATE_FUNC(NetworkClientView);

    const cocos2d::RefPtr<GameTableView>& getHostTableView() const { return m_hostTableView; }
    
private:
    cocos2d::RefPtr<GameTableView> m_hostTableView;
};

#endif // NETWORK_CLIENT_VIEW_H
