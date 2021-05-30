#ifndef NETWORK_HOST_VIEW_CONTROLLER_H
#define NETWORK_HOST_VIEW_CONTROLLER_H

#include "Core/Injector.h"
#include "cocos-ext.h"
#include "Network/DrudgeNet/include/DataTypes.h"
#include "Game/Shared/GameMode.h"

class ClientModel;
class GameSettings;
class INetworkController;
class NetworkHostView;
class NetworkModel;
class InputActionEvent;

namespace Net {
    class Stream;
    class TransportLAN;
    class Message;
}

class NetworkHostViewController
: public cocos2d::extension::TableViewDataSource
, public cocos2d::extension::TableViewDelegate
, public cocos2d::ui::EditBoxDelegate
{
public:
    NetworkHostViewController(std::shared_ptr<NetworkModel> networkModel,
                              std::shared_ptr<INetworkController> networkController,
                              std::shared_ptr<GameSettings> gameSettings,
                              std::shared_ptr<ClientModel> clientModel);
    ~NetworkHostViewController();
    
    void initialize();
    void terminate();
    
    void update(float deltaTime);

    void setView(NetworkHostView* view);
    
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
    struct ClientCellData {
        enum State {
            DISCONNECTED,
            HANDSHAKE,
            READY,
        } state;
        std::string name;
        std::string address;
    };
    
    std::shared_ptr<NetworkModel> m_model;
    std::shared_ptr<INetworkController> m_networkController;
    std::shared_ptr<GameSettings> m_gameSettings;
    std::shared_ptr<ClientModel> m_clientModel;

    NetworkHostView* m_view;
    
    float m_refreshClientsTimer;
    bool m_startGame;
    
    std::map<uint8_t, ClientCellData> m_clientData;
    
    GameMode::Config m_gameModeConfig;
    
    void onReadyButton(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
    void onSendButton(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
    void onBackToMainMenuButton(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
    void onInputAction(const InputActionEvent& event);

    void onPlayerInfoReceived(const std::shared_ptr<Net::Message>& message,
                              const Net::NodeID nodeID);
    void onPlayerReadyReceived(const std::shared_ptr<Net::Message>& message,
                               const Net::NodeID nodeID);
    void onChatMessageReceived(const std::shared_ptr<Net::Message>& message,
                               const Net::NodeID nodeID);
    
    void checkForAllReady();
    void startGame();
    void propagateChatMessage(const std::string& text, const Net::NodeID senderID);
};

#endif /* NETWORK_HOST_VIEW_CONTROLLER_H */
