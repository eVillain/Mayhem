#include "NetworkHostViewController.h"
#include "NetworkHostView.h"
#include "NetworkChatView.h"
#include "NetworkController.h"
#include "TransportLAN.h"
#include "Core/Injector.h"
#include "InitClientCommand.h"

#include "NetworkModel.h"
#include "NetworkView.h"
#include "NetworkHostClientCell.h"
#include "NetworkHostGameModeCell.h"
#include "NetworkHostGameLevelCell.h"
#include "Network/NetworkMessages.h"
#include "GameViewConstants.h"
#include "BackToMainMenuEvent.h"
#include "Core/Dispatcher.h"

NetworkHostViewController::NetworkHostViewController()
: m_model(nullptr)
, m_networkController(nullptr)
, m_view(nullptr)
, m_refreshClientsTimer(0)
, m_startGame(false)
{
    m_gameModeConfig.type = GameModeType::GAME_MODE_DEATHMATCH;
    m_gameModeConfig.maxPlayers = 100;
    m_gameModeConfig.tickRate = 20;
    m_gameModeConfig.playersPerTeam = 1;
    m_gameModeConfig.level = "BitTileMap.tmx";
}

NetworkHostViewController::~NetworkHostViewController()
{
}

void NetworkHostViewController::initialize()
{
    Injector& injector = Injector::globalInjector();
    m_model = injector.getInstance<NetworkModel>();
    m_networkController = injector.getInstance<NetworkController>();
    
    m_networkController->addMessageCallback(MessageTypes::MESSAGE_TYPE_CLIENT_READY, std::bind(&NetworkHostViewController::onPlayerReadyReceived, this, std::placeholders::_1, std::placeholders::_2));
    
    m_networkController->addMessageCallback(MessageTypes::MESSAGE_TYPE_CLIENT_CHAT_MESSAGE, std::bind(&NetworkHostViewController::onChatMessageReceived, this, std::placeholders::_1, std::placeholders::_2));
    
    m_networkController->setControllerMessageCallback([this](const std::string& message){
        m_view->getChatView()->addMessage(message, 0);
    });
    
    auto glview = cocos2d::Director::getInstance()->getOpenGLView();
    glview->setViewName("Mayhem Royale - Host");
    
    cocos2d::Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);
}

void NetworkHostViewController::terminate()
{
    cocos2d::Director::getInstance()->getScheduler()->unscheduleUpdate(this);
    m_networkController->removeMessageCallback(MessageTypes::MESSAGE_TYPE_CLIENT_READY);
    m_networkController->removeMessageCallback(MessageTypes::MESSAGE_TYPE_CLIENT_CHAT_MESSAGE);
    
    m_networkController->terminate();
}

void NetworkHostViewController::update(float deltaTime)
{
    m_networkController->receiveMessages();
    m_networkController->update(deltaTime);
    
    if (m_networkController->isBroadcasting())
    {
        m_refreshClientsTimer += deltaTime;
        const float CLIENT_REFRESH_TIME = 1.f;
        if (m_refreshClientsTimer > CLIENT_REFRESH_TIME)
        {
            m_refreshClientsTimer -= CLIENT_REFRESH_TIME;
            
            bool clientDataUpdated = false;
            auto transport = m_networkController->getTransport();
            const int maxNodes = transport->GetMaxNodes();
            for (int nodeID = 0; nodeID < maxNodes; nodeID++)
            {
                auto it = m_clientData.find(nodeID);

                if (!transport->IsNodeConnected(nodeID))
                {
                    if (it != m_clientData.end() &&
                        it->second.state != ClientCellData::State::DISCONNECTED)
                    {
                        it->second.state = ClientCellData::State::DISCONNECTED;
                        clientDataUpdated = true;
                    }
                    continue;
                }
                
                const Net::Address& address = transport->GetNodeAddress(nodeID);
                
                if (it == m_clientData.end())
                {
                    if (nodeID == 0)
                    {
                        m_view->showReadyButton(true);
                    }
                    ClientCellData data;
                    data.state = ClientCellData::State::HANDSHAKE;
                    std::ostringstream nameStream;
                    nameStream << "NodeID: " << std::to_string(nodeID);;
                    data.name = nameStream.str();
                    std::ostringstream addressStream;
                    addressStream << "Address: " <<
                        std::to_string(address.GetA()) << "." <<
                        std::to_string(address.GetB()) << "." <<
                        std::to_string(address.GetC()) << "." <<
                        std::to_string(address.GetD()) << ":" <<
                        std::to_string(address.GetPort());
                    data.address = addressStream.str();
                    m_clientData[nodeID] = data;
                    clientDataUpdated = true;
                }
            }
            
            if (clientDataUpdated)
            {
                m_view->getClientTableView()->getTable()->reloadData();
            }
        }
    }
    
    checkForAllReady();

    if (m_startGame)
    {
        startGame();
        m_networkController->sendMessages();
        
        terminate();
        
        InitClientCommand initGame(InitClientCommand::Mode::HOST, m_gameModeConfig);
        initGame.run();
        return;
    }

    m_networkController->sendMessages();
}

void NetworkHostViewController::setView(NetworkHostView* view)
{
    m_view = view;
    m_view->setupHostUI(this);
    m_view->setupChatView();
    m_view->getChatView()->getMessageEditBox()->setDelegate(this);
    m_view->getChatView()->getSendMessageButton()->addTouchEventListener(CC_CALLBACK_2(NetworkHostViewController::onSendButton, this));
    m_view->getReadyButton()->addTouchEventListener(CC_CALLBACK_2(NetworkHostViewController::onReadyButton, this));
    m_view->getExitButton()->addTouchEventListener(CC_CALLBACK_2(NetworkHostViewController::onBackToMainMenuButton, this));
    
    m_view->getClientTableView()->getTable()->reloadData();
    m_view->getGameTypeTableView()->getTable()->reloadData();
}

cocos2d::extension::TableViewCell* NetworkHostViewController::tableCellAtIndex(cocos2d::extension::TableView* table,
                                                                               ssize_t idx)
{
    if (table == m_view->getClientTableView()->getTable())
    {
        ClientCellData& data = m_clientData[idx];
        
        NetworkHostClientCell* cell = static_cast<NetworkHostClientCell*>(table->dequeueCell());
        if (!cell)
        {
            cell = NetworkHostClientCell::create();
        }
        cell->getHostNameLabel()->setString(data.name);
        cell->getHostAddressLabel()->setString(data.address);
        const cocos2d::Color3B CLIENT_STATE_COLOR = data.state == ClientCellData::State::READY ? cocos2d::Color3B::GREEN :
                                                    data.state == ClientCellData::State::DISCONNECTED ? cocos2d::Color3B::RED :
                                                        cocos2d::Color3B::GRAY;
        cell->getReadyIndicator()->setColor(CLIENT_STATE_COLOR);
        cell->refreshPositions();
        
        return cell;
    }
    else if (table == m_view->getGameTypeTableView()->getTable())
    {
        NetworkHostGameModeCell* cell = static_cast<NetworkHostGameModeCell*>(table->dequeueCell());
        if (!cell)
        {
            cell = NetworkHostGameModeCell::create();
        }

        cell->getGameModeLabel()->setString(GameMode::getGameModeName((GameModeType)idx));
        const cocos2d::Color3B GAME_MODE_SELECTED_COLOR = m_gameModeConfig.type == (GameModeType)idx ? cocos2d::Color3B::GREEN : cocos2d::Color3B::GRAY;
        cell->getSelectedIndicator()->setColor(GAME_MODE_SELECTED_COLOR);
        cell->refreshPositions();
        
        return cell;
    }
    else if (table == m_view->getGameLevelTableView()->getTable())
    {
        NetworkHostGameLevelCell* cell = static_cast<NetworkHostGameLevelCell*>(table->dequeueCell());
        if (!cell)
        {
            cell = NetworkHostGameLevelCell::create();
        }

        cell->getGameLevelLabel()->setString(m_gameModeConfig.level);
        const cocos2d::Color3B GAME_MODE_SELECTED_COLOR = m_gameModeConfig.type == (GameModeType)idx ? cocos2d::Color3B::GREEN : cocos2d::Color3B::GRAY;
        cell->getSelectedIndicator()->setColor(GAME_MODE_SELECTED_COLOR);
        cell->refreshPositions();
        
        return cell;
    }
    
    return nullptr;
}

ssize_t NetworkHostViewController::numberOfCellsInTableView(cocos2d::extension::TableView* table)
{
    if (table == m_view->getClientTableView()->getTable())
    {
        return m_clientData.size();
    }
    else if (table == m_view->getGameTypeTableView()->getTable())
    {
        return GameModeType::GAME_MODE_COUNT;
    }
    else if (table == m_view->getGameLevelTableView()->getTable())
    {
        return 1;
    }
    return 0;
}

void NetworkHostViewController::tableCellTouched(cocos2d::extension::TableView* table,
                                                 cocos2d::extension::TableViewCell* cell)
{
    if (table == m_view->getGameTypeTableView()->getTable())
    {
        m_gameModeConfig.type = (GameModeType)cell->getIdx();
        m_view->getGameTypeTableView()->getTable()->reloadData();
        
        // TODO: Show config options for selected game mode
    }
    CCLOG("NetworkHostViewController::tableCellTouched %zi\n", cell->getIdx());
}

cocos2d::Size NetworkHostViewController::tableCellSizeForIndex(cocos2d::extension::TableView* table, ssize_t idx)
{
    if (table == m_view->getGameTypeTableView()->getTable())
    {
        return GameViewConstants::GAMEMODE_CELL_SIZE;
    }
    return GameViewConstants::HOST_CELL_SIZE;
}

cocos2d::Size NetworkHostViewController::cellSizeForTable(cocos2d::extension::TableView* table)
{
    if (table == m_view->getGameTypeTableView()->getTable())
    {
        return GameViewConstants::GAMEMODE_CELL_SIZE;
    }
    return GameViewConstants::HOST_CELL_SIZE;
}

void NetworkHostViewController::editBoxEditingDidEndWithAction(cocos2d::ui::EditBox* editBox,
                                                               EditBoxEndAction action)
{
    if (action == EditBoxEndAction::RETURN)
    {
        onSendButton(nullptr, cocos2d::ui::Widget::TouchEventType::ENDED);
    }
}

void NetworkHostViewController::onReadyButton(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type)
{
    if (type != cocos2d::ui::Widget::TouchEventType::ENDED)
    {
        return;
    }
    auto it = m_clientData.find(0);
    if (it!=m_clientData.end())
    {
        it->second.state = ClientCellData::State::READY;
        m_view->getClientTableView()->getTable()->reloadData();
    }
}

void NetworkHostViewController::onSendButton(cocos2d::Ref* ref,
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
    
    m_view->getChatView()->addMessage(text, 0);
    propagateChatMessage(text, 0);
    
    m_view->getChatView()->getMessageEditBox()->setText("");
}

void NetworkHostViewController::onBackToMainMenuButton(cocos2d::Ref *ref, cocos2d::ui::Widget::TouchEventType type)
{
    if (type != cocos2d::ui::Widget::TouchEventType::ENDED)
    {
        return;
    }
    
    terminate();
    
    Dispatcher::globalDispatcher().dispatch(BackToMainMenuEvent());
}

void NetworkHostViewController::onPlayerReadyReceived(const std::shared_ptr<Net::Message>& message,
                                                      const Net::NodeID nodeID)
{
    auto it = m_clientData.find(nodeID);
    if (it!=m_clientData.end())
    {
        it->second.state = ClientCellData::State::READY;
        m_view->getClientTableView()->getTable()->reloadData();
        
        std::shared_ptr<Net::Message> readyResponseMessage = std::make_shared<ClientReadyMessage>();
        m_networkController->sendMessage(nodeID, readyResponseMessage);
    }
}

void NetworkHostViewController::onChatMessageReceived(const std::shared_ptr<Net::Message>& message,
                                                      const Net::NodeID nodeID)
{
    if (nodeID == 0)
    {
        return;
    }
    if (auto chatMessage = std::dynamic_pointer_cast<ClientChatMessage>(message))
    {
        m_view->getChatView()->addMessage(chatMessage->text, nodeID);
        propagateChatMessage(chatMessage->text, nodeID);
    }
}

void NetworkHostViewController::checkForAllReady()
{
    if (m_clientData.empty())
    {
        return;
    }
    
    for (auto clientData : m_clientData)
    {
        if (clientData.second.state != ClientCellData::State::READY)
        {
            return;
        }
    }
    
    m_startGame = true;
}

void NetworkHostViewController::startGame()
{
    auto transport = m_networkController->getTransport();
    const int maxNodes = transport->GetMaxNodes();
    for (int nodeID = 1; nodeID < maxNodes; nodeID++)
    {
        if (!transport->IsNodeConnected(nodeID))
        {
            continue;
        }
        
        std::shared_ptr<ServerLoadLevelMessage> loadLevelMessage = std::make_shared<ServerLoadLevelMessage>();
        loadLevelMessage->modeType = m_gameModeConfig.type;
        loadLevelMessage->maxPlayers = m_gameModeConfig.maxPlayers;
        loadLevelMessage->playersPerTeam = m_gameModeConfig.playersPerTeam;
        loadLevelMessage->level = m_gameModeConfig.level;
        
        std::shared_ptr<Net::Message> message = loadLevelMessage;
        m_networkController->sendMessage(nodeID, message, true);
    }
}

void NetworkHostViewController::propagateChatMessage(const std::string& text, const Net::NodeID senderID)
{
    std::shared_ptr<ServerChatMessage> chatResponseMessage = std::make_shared<ServerChatMessage>();
    chatResponseMessage->text = text;
    chatResponseMessage->playerID = senderID;
    std::shared_ptr<Net::Message> message = chatResponseMessage;

    auto transport = m_networkController->getTransport();
    const int maxNodes = transport->GetMaxNodes();
    for (int nodeID = 1; nodeID < maxNodes; nodeID++)
    {
        if (!transport->IsNodeConnected(nodeID))
        {
            continue;
        }
        m_networkController->sendMessage(nodeID, message);
    }
}
