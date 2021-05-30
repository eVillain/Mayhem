#ifndef NETWORK_CLIENT_CONTROLLER_H
#define NETWORK_CLIENT_CONTROLLER_H

#include "Core/Injector.h"
#include "Network/DrudgeNet/include/DataTypes.h"
#include "cocos-ext.h"

namespace Net {
    class Message;
};

class InputActionEvent;
class INetworkController;
class NetworkClientView;
class GameSettings;
class ClientModel;

class NetworkClientViewController
: public cocos2d::extension::TableViewDataSource
, public cocos2d::extension::TableViewDelegate
, public cocos2d::ui::EditBoxDelegate
{
public:
    NetworkClientViewController(std::shared_ptr<INetworkController> networkController,
                                std::shared_ptr<GameSettings> gameSettings,
                                std::shared_ptr<ClientModel> clientModel);
    ~NetworkClientViewController();
    
    void initialize();
    void terminate();
    
    void update(float deltaTime);

    void setView(NetworkClientView* view);
    
    cocos2d::extension::TableViewCell* tableCellAtIndex(cocos2d::extension::TableView *table,
                                                        ssize_t idx) override;
    ssize_t numberOfCellsInTableView(cocos2d::extension::TableView *table) override;
    
    void tableCellTouched(cocos2d::extension::TableView* table,
                          cocos2d::extension::TableViewCell* cell) override;
    
    cocos2d::Size tableCellSizeForIndex(cocos2d::extension::TableView* table, ssize_t idx) override;
    
    cocos2d::Size cellSizeForTable(cocos2d::extension::TableView* table) override;
    
    void editBoxReturn(cocos2d::ui::EditBox* editBox) override {};
    void editBoxEditingDidEndWithAction(cocos2d::ui::EditBox* editBox,
                                        EditBoxEndAction action) override;
private:
    static const int BUFFER_SIZE;

    struct HostData {
        std::string name;
        std::string address;
    };
    
    std::shared_ptr<INetworkController> m_networkController;
    std::shared_ptr<GameSettings> m_gameSettings;
    std::shared_ptr<ClientModel> m_clientModel;
    NetworkClientView* m_view;
    
    float m_refreshHostsTimer;
    
    std::vector<HostData> m_hostData;
    
    void registerNetworkCallbacks();
    void unregisterNetworkCallbacks();
    void onConnected(const Net::NodeID nodeID);
    void onDisconnected(const Net::NodeID nodeID);
    
    void onReadyButton(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
    void onSendButton(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
    void onBackToMainMenuButton(cocos2d::Ref *ref, cocos2d::ui::Widget::TouchEventType type);
    void onInputAction(const InputActionEvent& event);

    void onLoadLevelReceived(const std::shared_ptr<Net::Message>& message,
                             const Net::NodeID nodeID);
    void onChatMessageReceived(const std::shared_ptr<Net::Message>& message,
                               const Net::NodeID nodeID);
    void onServerInfoReceived(const std::shared_ptr<Net::Message>& data,
                              const Net::NodeID nodeID);
};

#endif /* NETWORK_CLIENT_CONTROLLER_H */
