#ifndef NETWORK_HOST_VIEW_H
#define NETWORK_HOST_VIEW_H

#include "NetworkView.h"
#include "GameTableView.h"
#include "cocos-ext.h"

class NetworkHostViewController;
class GameTableView;

class NetworkHostView : public NetworkView
{
public:
    NetworkHostView();
    
    virtual bool init();
    
    CREATE_FUNC(NetworkHostView);

    const cocos2d::RefPtr<GameTableView>& getClientTableView() const { return m_clientTableView; }
    const cocos2d::RefPtr<GameTableView>& getGameTypeTableView() const { return m_gameTypeTableView; }
    const cocos2d::RefPtr<GameTableView>& getGameLevelTableView() const { return m_gameLevelTableView; }

    void setupHostUI(NetworkHostViewController* controller);
    
private:
    cocos2d::RefPtr<GameTableView> m_clientTableView;
    cocos2d::RefPtr<GameTableView> m_gameTypeTableView;
    cocos2d::RefPtr<GameTableView> m_gameLevelTableView;

};

#endif // NETWORK_HOST_VIEW_H
