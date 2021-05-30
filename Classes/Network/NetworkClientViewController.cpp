#include "NetworkClientViewController.h"

#include "BackToMainMenuEvent.h"
#include "ClientModel.h"
#include "Core/Dispatcher.h"
#include "GameSettings.h"
#include "GameViewConstants.h"
#include "INetworkController.h"
#include "InitClientCommand.h"
#include "InputActionEvent.h"
#include "InputConstants.h"
#include "Network/NetworkMessages.h"
#include "NetworkChatView.h"
#include "NetworkClientHostCell.h"
#include "NetworkClientView.h"
#include "NetworkConstants.h"
#include "NetworkView.h"
#include "ShutdownNetworkClientCommand.h"
#include "Transport.h"

NetworkClientViewController::NetworkClientViewController(std::shared_ptr<INetworkController> networkController,
                                                         std::shared_ptr<GameSettings> gameSettings,
                                                         std::shared_ptr<ClientModel> clientModel)
: m_networkController(networkController)
, m_gameSettings(gameSettings)
, m_clientModel(clientModel)
, m_refreshHostsTimer(0.f)
{
    printf("NetworkClientViewController:: constructor %p\n", this);
}

NetworkClientViewController::~NetworkClientViewController()
{
    printf("NetworkClientViewController:: destructor %p\n", this);
}

void NetworkClientViewController::initialize()
{
    registerNetworkCallbacks();
    
    cocos2d::Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);
    
    auto glview = cocos2d::Director::getInstance()->getOpenGLView();
    glview->setViewName("Mayhem Royale - Client");
}

void NetworkClientViewController::terminate()
{
    unregisterNetworkCallbacks();
    m_networkController->setNodeConnectedCallback(nullptr);
    m_networkController->setNodeDisconnectedCallback(nullptr);

    cocos2d::Director::getInstance()->getScheduler()->unscheduleUpdate(this);
    
    m_networkController->terminate();
}

void NetworkClientViewController::update(float deltaTime)
{
    if (m_networkController->isListening())
    {
        m_refreshHostsTimer += deltaTime;
        const float HOST_REFRESH_TIME = 1.f;
        if (m_refreshHostsTimer > HOST_REFRESH_TIME)
        {
            m_refreshHostsTimer -= HOST_REFRESH_TIME;
            
            m_hostData.clear();
            
            const int hostCount = m_networkController->getTransport()->GetLobbyEntryCount();
            for (int i = 0; i < hostCount; i++)
            {
                Net::Transport::LobbyEntry entry;
                if (!m_networkController->getTransport()->GetLobbyEntryAtIndex(i, entry))
                {
                    continue;
                }

                HostData data;
                data.name = entry.name;
                data.address = entry.address.GetString();
                m_hostData.push_back(data);
            }
            
            if (hostCount)
            {
                m_view->getHostTableView()->getTable()->reloadData();
            }
        }
    }
    else if (m_networkController->isConnected())
    {
        m_networkController->receiveMessages();
        m_networkController->sendMessages();
    }
    
    m_networkController->update(deltaTime);
}

void NetworkClientViewController::setView(NetworkClientView* view)
{
    m_view = view;
    m_view->setupClientUI(this);
    m_view->getReadyButton()->addTouchEventListener(CC_CALLBACK_2(NetworkClientViewController::onReadyButton, this));
    m_view->getExitButton()->addTouchEventListener(CC_CALLBACK_2(NetworkClientViewController::onBackToMainMenuButton, this));
    
    Dispatcher::globalDispatcher().addListener<InputActionEvent>(std::bind(&NetworkClientViewController::onInputAction,
                                                                           this, std::placeholders::_1),
                                                                 this);
}

cocos2d::extension::TableViewCell* NetworkClientViewController::tableCellAtIndex(cocos2d::extension::TableView *table,
                                                                                 ssize_t idx)
{
    
    NetworkClientHostCell* cell = static_cast<NetworkClientHostCell*>(table->dequeueCell());
    if (!cell)
    {
        cell = NetworkClientHostCell::create();
    }
    cell->getHostNameLabel()->setString(m_hostData[idx].name);
    cell->getHostAddressLabel()->setString(m_hostData[idx].address);
    
    cell->refreshPositions();
    
    return cell;
}

ssize_t NetworkClientViewController::numberOfCellsInTableView(cocos2d::extension::TableView *table)
{
    return m_hostData.size();
}

void NetworkClientViewController::tableCellTouched(cocos2d::extension::TableView* table,
                                                   cocos2d::extension::TableViewCell* cell)
{
    auto idx = cell->getIdx();
    if (m_hostData.size() <= idx)
    {
        return;
    }
    
    m_networkController->stop();
    
    m_networkController->setNodeConnectedCallback(std::bind(&NetworkClientViewController::onConnected, this, std::placeholders::_1));
    m_networkController->setNodeDisconnectedCallback(std::bind(&NetworkClientViewController::onDisconnected, this, std::placeholders::_1));
    m_networkController->join(m_hostData[idx].name);
    
    m_hostData.clear();
    m_view->getHostTableView()->getTable()->reloadData();
}

cocos2d::Size NetworkClientViewController::tableCellSizeForIndex(cocos2d::extension::TableView* table, ssize_t idx)
{
    return GameViewConstants::CLIENT_CELL_SIZE;
}

cocos2d::Size NetworkClientViewController::cellSizeForTable(cocos2d::extension::TableView* table)
{
    return GameViewConstants::CLIENT_CELL_SIZE;
}

void NetworkClientViewController::registerNetworkCallbacks()
{
    m_networkController->addMessageCallback(MessageTypes::MESSAGE_TYPE_SERVER_LOAD_LEVEL,
                                            std::bind(&NetworkClientViewController::onLoadLevelReceived, this,
                                                      std::placeholders::_1, std::placeholders::_2));
    m_networkController->addMessageCallback(MessageTypes::MESSAGE_TYPE_SERVER_CHAT_MESSAGE,
                                            std::bind(&NetworkClientViewController::onChatMessageReceived, this,
                                                      std::placeholders::_1, std::placeholders::_2));
    m_networkController->addMessageCallback(MessageTypes::MESSAGE_TYPE_SERVER_INFO,
                                            std::bind(&NetworkClientViewController::onServerInfoReceived, this,
                                                      std::placeholders::_1, std::placeholders::_2));
}

void NetworkClientViewController::unregisterNetworkCallbacks()
{
    m_networkController->removeMessageCallback(MessageTypes::MESSAGE_TYPE_SERVER_LOAD_LEVEL);
    m_networkController->removeMessageCallback(MessageTypes::MESSAGE_TYPE_SERVER_CHAT_MESSAGE);
}

void NetworkClientViewController::onConnected(const Net::NodeID nodeID)
{
    if (nodeID == m_networkController->getLocalNodeID())
    {
        m_hostData.clear();
        m_view->setHostTableVisible(false);
        
        m_view->showReadyButton(true);
        m_view->setupChatView();
        m_view->getChatView()->getMessageEditBox()->setDelegate(this);
        m_view->getChatView()->getSendMessageButton()->addTouchEventListener(CC_CALLBACK_2(NetworkClientViewController::onSendButton, this));
        m_view->getChatView()->addMessage("Connected to server as Player: " + std::to_string(nodeID), "CLIENT");

        const cocos2d::Value& playerNameSetting = m_gameSettings->getValue(GameSettings::SETTING_PLAYER_NAME, cocos2d::Value(""));
        // Send client info to server:
        std::shared_ptr<ClientInfoMessage> infoMessage = std::make_shared<ClientInfoMessage>();
        infoMessage->name = playerNameSetting.asString();
        std::shared_ptr<Net::Message> message = infoMessage;
        m_networkController->sendMessage(0, message);
    }
    else
    {
        m_view->getChatView()->addMessage("Player " + std::to_string(nodeID) + " connected to server", "HOST");
    }
}

void NetworkClientViewController::onDisconnected(const Net::NodeID nodeID)
{
    m_view->getReadyButton()->setEnabled(false);
    m_view->removeChatView();
}

void NetworkClientViewController::editBoxEditingDidEndWithAction(cocos2d::ui::EditBox* editBox,
                                                                 EditBoxEndAction action)
{
    if (action == EditBoxEndAction::RETURN)
    {
        onSendButton(nullptr, cocos2d::ui::Widget::TouchEventType::ENDED);
    }
}

void NetworkClientViewController::onReadyButton(cocos2d::Ref* ref,
                                                cocos2d::ui::Widget::TouchEventType type)
{
    if (!m_networkController->isConnected())
    {
        return;
    }
    if (type != cocos2d::ui::Widget::TouchEventType::ENDED)
    {
        return;
    }
    std::shared_ptr<Net::Message> playerReadyMessage = std::make_shared<ClientReadyMessage>();
    m_networkController->sendMessage(0, playerReadyMessage, true);
}

void NetworkClientViewController::onSendButton(cocos2d::Ref* ref,
                                                cocos2d::ui::Widget::TouchEventType type)
{
    if (!m_networkController->isConnected())
    {
        return;
    }
    if (type != cocos2d::ui::Widget::TouchEventType::ENDED)
    {
        return;
    }
    
    const std::string text = m_view->getChatView()->getMessageEditBox()->getText();
    if (text.length() == 0)
    {
        return;
    }
    std::shared_ptr<ClientChatMessage> playerChatMessage = std::make_shared<ClientChatMessage>();
    playerChatMessage->text = text;
    std::shared_ptr<Net::Message> message = playerChatMessage;
    m_networkController->sendMessage(0, message);
    
    m_view->getChatView()->getMessageEditBox()->setText("");
}

void NetworkClientViewController::onBackToMainMenuButton(cocos2d::Ref *ref, cocos2d::ui::Widget::TouchEventType type)
{
    if (type != cocos2d::ui::Widget::TouchEventType::ENDED)
    {
        return;
    }

    terminate();
    
    ShutdownNetworkClientCommand shutdownClient;
    shutdownClient.run();
    
    BackToMainMenuEvent back;
    Dispatcher::globalDispatcher().dispatch(back);
}

void NetworkClientViewController::onInputAction(const InputActionEvent& event)
{
    if (event.previousValue == 1.f || event.value < 1.f)
    {
        return;
    }
    if (event.action == InputConstants::ACTION_BACK)
    {
        onBackToMainMenuButton(nullptr, cocos2d::ui::Widget::TouchEventType::ENDED);
    }
}

void NetworkClientViewController::onLoadLevelReceived(const std::shared_ptr<Net::Message>& message,
                                                      const Net::NodeID nodeID)
{
    auto loadLevelMessage = std::dynamic_pointer_cast<ServerLoadLevelMessage>(message);
    if (!loadLevelMessage)
    {
        return;
    }

    CCLOG("client received LOAD_LEVEL from server %i, level %s",
          nodeID, loadLevelMessage->level.c_str());
    
    terminate();

    GameMode::Config config = {
        (GameModeType)loadLevelMessage->modeType,
        loadLevelMessage->tickRate,
        loadLevelMessage->maxPlayers,
        loadLevelMessage->playersPerTeam,
        loadLevelMessage->level
    };
    
    InitClientCommand initGame(InitClientCommand::Mode::CLIENT, config);
    initGame.run();
}

void NetworkClientViewController::onChatMessageReceived(const std::shared_ptr<Net::Message>& message,
                                                        const Net::NodeID nodeID)
{
    auto chatMessage = std::dynamic_pointer_cast<ServerChatMessage>(message);
    if (!chatMessage)
    {
        return;
    }

    CCLOG("client received CHAT_MESSAGE from server %i, sender: %i, message %s",
          nodeID, chatMessage->playerID, chatMessage->text.c_str());
    
    m_view->getChatView()->addMessage(chatMessage->text,
                                      m_clientModel->getPlayerName(chatMessage->playerID));
}

void NetworkClientViewController::onServerInfoReceived(const std::shared_ptr<Net::Message>& message,
                                                       const Net::NodeID nodeID)
{
    auto infoMessage = std::dynamic_pointer_cast<ServerInfoMessage>(message);
    if (!infoMessage)
    {
        return;
    }

    CCLOG("client received INFO_MESSAGE from server %i with %i players", nodeID, infoMessage->playerCount);
    
    for (size_t i = 0; i < infoMessage->playerCount; i++)
    {
        const uint8_t playerID = infoMessage->playerIDs.at(i);
        const std::string& name = infoMessage->names.at(i);
        m_clientModel->setPlayerName(playerID, name);
    }
}
