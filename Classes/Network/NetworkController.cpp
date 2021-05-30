#include "NetworkController.h"
#include "NetworkConstants.h"
#include "NetworkModel.h"
#include "NetworkMessageFactory.h"

#include "ReadStream.h"
#include "WriteStream.h"
#include "TransportLAN.h"
#include "ReliabilitySystem.h"
#include "Message.h"
#include "cocos2d.h"
#include "GameSettings.h"
#include "AddressResolver.h"

const int NetworkController::BUFFER_SIZE = 2 * 1024 * 1024;
const std::string NetworkController::SETTING_NETWORK_TYPE = "NetworkType";
const std::string NetworkController::SETTING_NETWORK_PROTOCOL_ID = "NetworkProtocolID";
const std::string NetworkController::SETTING_NETWORK_MASTER_SERVER_ADDRESS = "MasterServerAddress";
const std::string NetworkController::SETTING_NETWORK_MASTER_SERVER_PORT = "MasterServerConnectPort";
const std::string NetworkController::SETTING_NETWORK_MESH_PORT = "NetworkMeshPort";
const std::string NetworkController::SETTING_NETWORK_SERVER_PORT = "NetworkServerPort";
const std::string NetworkController::SETTING_NETWORK_CLIENT_PORT = "NetworkClientPort";
const std::string NetworkController::SETTING_NETWORK_MESH_SEND_RATE = "NetworkMeshSendRate";
const std::string NetworkController::SETTING_NETWORK_TIMEOUT = "NetworkTimeout";
const std::string NetworkController::SETTING_NETWORK_MAX_NODES = "NetworkMaxNodes";

NetworkController::NetworkController(std::shared_ptr<NetworkModel> model,
                                     std::shared_ptr<GameSettings> gameSettings)
: m_model(model)
, m_gameSettings(gameSettings)
, m_drudgeNet(nullptr)
, m_messageFactory(nullptr)
, m_readBuffer(nullptr)
, m_writeBuffer(nullptr)
, m_readStream(nullptr)
, m_writeStream(nullptr)
, m_mode(NetworkMode::HOST)
, m_isBroadcasting(false)
, m_isListening(false)
, m_isConnected(false)
{
    m_messageFactory = std::make_shared<NetworkMessageFactory>();
    auto messageFactory = std::static_pointer_cast<Net::MessageFactory>(m_messageFactory);
    m_drudgeNet = std::make_shared<Net::DrudgeNet>(messageFactory);
    printf("NetworkController:: constructor %p\n", this);
}

NetworkController::~NetworkController()
{
    printf("NetworkController:: destructor %p\n", this);
}

void NetworkController::initialize(const NetworkMode mode)
{
    m_mode = mode;
    
    const cocos2d::Value& netTypeSetting = m_gameSettings->getValue(SETTING_NETWORK_TYPE, cocos2d::Value("LAN"));
    const cocos2d::Value& netProtocolIDSetting = m_gameSettings->getValue(SETTING_NETWORK_PROTOCOL_ID, cocos2d::Value(666666));
    const cocos2d::Value& masterServerAddressSetting = m_gameSettings->getValue(SETTING_NETWORK_MASTER_SERVER_ADDRESS, cocos2d::Value("0"));
    const cocos2d::Value& masterServerPortSetting = m_gameSettings->getValue(SETTING_NETWORK_MASTER_SERVER_PORT, cocos2d::Value(12345));
    const cocos2d::Value& meshPortSetting = m_gameSettings->getValue(SETTING_NETWORK_MESH_PORT, cocos2d::Value(12346));
    const cocos2d::Value& serverPortSetting = m_gameSettings->getValue(SETTING_NETWORK_SERVER_PORT, cocos2d::Value(12347));
    const cocos2d::Value& clientPortSetting = m_gameSettings->getValue(SETTING_NETWORK_CLIENT_PORT, cocos2d::Value(12348));
    const cocos2d::Value& meshSendRateSetting = m_gameSettings->getValue(SETTING_NETWORK_MESH_SEND_RATE, cocos2d::Value(5));
    const cocos2d::Value& timeoutSetting = m_gameSettings->getValue(SETTING_NETWORK_TIMEOUT, cocos2d::Value(10));
    const cocos2d::Value& maxNodesSetting = m_gameSettings->getValue(SETTING_NETWORK_MAX_NODES, cocos2d::Value(100));

    const std::string hostNameWithPort = masterServerAddressSetting.asString();
    const size_t colon = hostNameWithPort.find(":") + 1;
    const std::string portString = hostNameWithPort.substr(colon, hostNameWithPort.length() - colon);
    const std::string hostString = hostNameWithPort.substr(0, colon - 1);
    
    const Net::Address masterServerAddress = Net::AddressResolver::getAddressForHost(hostString, portString);
    const Net::Port masterServerConnectPort = masterServerPortSetting.asInt();
    const Net::Port meshPort = meshPortSetting.asInt();
    const Net::Port serverPort = serverPortSetting.asInt();
    const Net::Port clientPort = clientPortSetting.asInt();
    const Net::ProtocolID protocolID = netProtocolIDSetting.asInt();
    const float meshSendRate = meshSendRateSetting.asFloat();
    const float timeout = timeoutSetting.asFloat();
    const int32_t maxNodes = maxNodesSetting.asInt();
    
    if (netTypeSetting.asString() == "LAN")
    {
        m_drudgeNet->initialize(Net::TransportType::Transport_LAN,
                                masterServerAddress,
                                masterServerConnectPort,
                                meshPort,
                                serverPort,
                                clientPort,
                                protocolID,
                                meshSendRate,
                                timeout,
                                maxNodes);
    }
    else if (netTypeSetting.asString() == "IP")
    {
        m_drudgeNet->initialize(Net::TransportType::Transport_IP,
                                masterServerAddress,
                                masterServerConnectPort,
                                meshPort,
                                serverPort,
                                clientPort,
                                protocolID,
                                meshSendRate,
                                timeout,
                                maxNodes);
    }
    m_drudgeNet->setMessageReceivedCallback(std::bind(&NetworkController::onMessageReceived, this, std::placeholders::_1, std::placeholders::_2));
    m_drudgeNet->setReliableMessageAckedCallback(std::bind(&NetworkController::onMessageAcked, this, std::placeholders::_1, std::placeholders::_2));
    m_drudgeNet->setNodeConnectedCallback(std::bind(&NetworkController::onNodeConnected, this, std::placeholders::_1));
    m_drudgeNet->setNodeDisconnectedCallback(std::bind(&NetworkController::onNodeDisconnected, this, std::placeholders::_1));
    m_drudgeNet->setMasterServerConnectionCallback(std::bind(&NetworkController::onMasterSystemConnection, this, std::placeholders::_1));

    m_readBuffer = new unsigned char[BUFFER_SIZE];
    m_writeBuffer = new unsigned char[BUFFER_SIZE];
    
    m_readStream = std::make_shared<Net::ReadStream>(m_readBuffer, BUFFER_SIZE);
    m_writeStream = std::make_shared<Net::WriteStream>(m_writeBuffer, BUFFER_SIZE);
    
    if (m_mode == NetworkMode::HOST)
    {
        host(m_model->getHostName());
    }
    else if (m_mode == NetworkMode::CLIENT)
    {
        enterLobby();
    }
}

void NetworkController::terminate()
{
    delete [] m_readBuffer;
    m_readBuffer = nullptr;
    delete [] m_writeBuffer;
    m_writeBuffer = nullptr;
    m_drudgeNet->stop();
}

void NetworkController::update(float deltaTime)
{
    if (m_drudgeNet)
    {
        m_drudgeNet->update(deltaTime);
    }
}

void NetworkController::host(const std::string& name)
{
    m_drudgeNet->host(name);
    m_isBroadcasting = true;
}

void NetworkController::join(const std::string& host)
{
    m_drudgeNet->join(host);
}

void NetworkController::enterLobby()
{
    m_drudgeNet->enterLobby();
    m_isListening = true;
}

void NetworkController::stop()
{
    m_drudgeNet->stop();
    
    m_isBroadcasting = false;
    m_isListening = false;
}

Net::MessageID NetworkController::sendMessage(const Net::NodeID nodeID,
                                              std::shared_ptr<Net::Message>& message,
                                              bool reliable /*= false*/)
{
    if (m_mode == NetworkMode::HOST &&
        nodeID == m_drudgeNet->getTransport()->GetLocalNodeId())
    {
        // local client loopback
        onMessageReceived(message, nodeID);
        return 0;
    }
    return m_drudgeNet->sendMessage(nodeID, message, reliable);
}

void NetworkController::receiveMessages()
{
    m_drudgeNet->receiveMessages();
}

void NetworkController::sendMessages()
{
    m_drudgeNet->sendMessages();
}

void NetworkController::setDeltaDataCallback(std::function<const SnapshotData&(const uint32_t)> dataCallback)
{
    m_messageFactory->setDeltaDataCallback(dataCallback);
}

float NetworkController::getRoundTripTime(const Net::NodeID nodeID)
{
    return m_drudgeNet->getTransport()->GetReliability(nodeID)->GetRoundTripTime();
}

const Net::NodeID NetworkController::getLocalNodeID() const
{
    return m_drudgeNet->getTransport()->GetLocalNodeId();
}

void NetworkController::onNodeConnected(const Net::NodeID nodeID)
{
    m_isConnected = true;
    if (m_onNodeConnectedCallback)
    {
        m_onNodeConnectedCallback(nodeID);
    }
}

void NetworkController::onNodeDisconnected(const Net::NodeID nodeID)
{
    m_isConnected = true;
    if (m_onNodeDisconnectedCallback)
    {
        m_onNodeDisconnectedCallback(nodeID);
    }
}

void NetworkController::onMessageReceived(const std::shared_ptr<Net::Message>& message,
                                          const Net::NodeID nodeID)
{
    if (m_messageReceivedCallback)
    {
        m_messageReceivedCallback(message, nodeID);
    }
    
    auto responder = m_dispatchMap.find(message->getType());
    if (responder != m_dispatchMap.end())
    {
        responder->second(message, nodeID);
    }
    else
    {
        CCLOG("received packetType %i (%s) from node %i with no handler\n",
              message->getType(), NetworkMessageFactory::getNameForType(message->getType()).c_str(), nodeID);
    }
}

void NetworkController::onMessageAcked(const Net::NodeID nodeID,
                                       const Net::MessageID messageID)
{
    if (m_onReliableMessageAckedCallback)
    {
        m_onReliableMessageAckedCallback(nodeID, messageID);
    }
}

void NetworkController::onMasterSystemConnection(bool connected)
{
    controllerMessage(connected ? "Master Server connected!" : "Master Server disconnected!");
}
