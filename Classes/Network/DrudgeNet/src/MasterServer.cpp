#include "MasterServer.h"

#include "CRC32.h"
#include "DataConstants.h"
#include "MasterServerMessages.h"
#include "ReadStream.h"
#include "Serialization.h"
#include "Socket.h"
#include "WriteStream.h"
#include <iostream>
#include <algorithm>

namespace Net {
MasterServer::MasterServer(const Net::Port serverPort,
                           const Net::ProtocolID protocolID,
                           const float timeOut)
: m_socket(nullptr)
, m_readStream(nullptr)
, m_writeStream(nullptr)
, m_readBuffer(nullptr)
, m_writeBuffer(nullptr)
, m_infoCallback(nullptr)
, m_onConnectedCallback(nullptr)
, m_onDisconnectedCallback(nullptr)
, m_port(serverPort)
, m_protocolID(protocolID)
, m_timeout(timeOut)
, m_running(false)
{
    m_socket = std::unique_ptr<Net::Socket>(new Net::Socket());
    m_readBuffer = new unsigned char[Net::BUFFER_SIZE_BYTES];
    m_writeBuffer = new unsigned char[Net::BUFFER_SIZE_BYTES];
    memset(m_readBuffer, 0, Net::BUFFER_SIZE_BYTES);
    memset(m_writeBuffer, 0, Net::BUFFER_SIZE_BYTES);
    m_readStream = std::unique_ptr<Net::ReadStream>(new Net::ReadStream(m_readBuffer, Net::BUFFER_SIZE_BYTES));
    m_writeStream = std::unique_ptr<Net::WriteStream>(new Net::WriteStream(m_writeBuffer, Net::BUFFER_SIZE_BYTES));
}

MasterServer::~MasterServer()
{
    delete [] m_readBuffer;
    delete [] m_writeBuffer;
}

bool MasterServer::start()
{
    if (m_running)
    {
        return false;
    }
    
    onInfo("MasterServer: start connection on port: " + std::to_string(m_port));
    onInfo("MasterServer: protocolID: " + std::to_string(m_protocolID));

    if (!m_socket->Open(m_port))
    {
        return false;
    }
    m_running = true;
    
    return true;
}

void MasterServer::stop()
{
    onInfo("MasterServer: stop connection");
    m_socket->Close();
    m_running = false;
    
    m_connectedHosts.clear();
    m_connectedClients.clear();
}

void MasterServer::update(const float deltaTime)
{
    if (!m_running)
    {
        return;
    }
    readSocketData();
    
    updateConnections(deltaTime);
}

void MasterServer::readSocketData()
{
    while (true)
    {
        Net::Address sender;
        const int64_t receivedBytes = m_socket->Receive(sender, m_readBuffer, Net::BUFFER_SIZE_BYTES);
        if (receivedBytes <= 0)
        {
            break;
        }
        processReceivedData(sender, receivedBytes);
    }
}

void MasterServer::processReceivedData(const Net::Address& sender,
                                       const int64_t receivedBytes)
{
    if (!validateDataHeader(receivedBytes))
    {
        onInfo("MasterServer: received invalid header from " + sender.GetString());
        memset(m_readBuffer, 0, receivedBytes);
        m_readStream->Clear();
        return;
    }
    
    Net::MessageType messageType = 0;
    m_readStream->SerializeByte(messageType);
    if (messageType == PingMessageType)
    {
        std::string pingy;
        m_readStream->SerializeString(pingy);
        if (pingy != "pong")
        {
            sendPing(sender, true);
        }
    }
    else if (messageType == GameHostStatusUpdateMessageType)
    {
        onGameHostUpdate(sender);
    }
    else if (messageType == GameListRequestMessageType)
    {
        onGameListRequest(sender);
    }
    else if (messageType == GameClientConnectionRequestMessageType)
    {
        onGameConnectionRequest(sender);
    }

    memset(m_readBuffer, 0, receivedBytes);
    m_readStream->Clear();
}

void MasterServer::updateConnections(const float deltaTime)
{
    for (auto& gameHost : m_connectedHosts)
    {
        gameHost.timeSinceUpdateReceived += deltaTime;
        gameHost.timeSinceUpdateSent += deltaTime;
        if (gameHost.timeSinceUpdateReceived >= m_timeout)
        {
            if (m_onDisconnectedCallback)
            {
                onInfo("MasterServer: game host disconnected " + gameHost.address.GetString());
                m_onDisconnectedCallback(gameHost.address);
            }
        }
        else
        {
            const float PING_TIME = 2.f;
            if (gameHost.timeSinceUpdateSent >= PING_TIME)
            {
                gameHost.timeSinceUpdateSent = 0.f;
                sendPing(gameHost.address, false);
            }
        }
    }

    std::remove_if(m_connectedHosts.begin(),
                   m_connectedHosts.end(),
                   [this](const GameHostMasterData& gameHost){
        return gameHost.timeSinceUpdateReceived >= m_timeout;
    });
    
    for (auto& client : m_connectedClients)
    {
        client.timeSinceUpdate += deltaTime;
    }
    
    std::remove_if(m_connectedClients.begin(),
                   m_connectedClients.end(),
                   [this](const GameClientData& gameClient){
        return gameClient.timeSinceUpdate >= m_timeout;
    });
}

bool MasterServer::validateDataHeader(const int64_t receivedBytes)
{
    if (receivedBytes < 5)
    {
        return false;
    }

    uint32_t crcHeader = 0;
    Net::Serialization::ReadInteger(m_readBuffer, crcHeader);
    Net::Serialization::WriteInteger(m_readBuffer, m_protocolID);
    uint32_t crcPacket = 0;
    crcPacket = crc32c(crcPacket, m_readBuffer, receivedBytes);
    
    uint32_t discard = 0;
    m_readStream->SerializeInteger(discard);

    return (crcHeader == crcPacket);
}

void MasterServer::writeHeaderData(const int dataBytes)
{
    uint32_t crc = 0;
    crc = crc32c(crc, m_writeBuffer, dataBytes);
    Net::Serialization::WriteInteger(m_writeBuffer, crc);
}

void MasterServer::sendMessage(const std::shared_ptr<Net::Message>& message,
                               const Net::Address& address)
{
    uint32_t discard = 0;
    m_writeStream->SerializeInteger(discard);
    Net::Serialization::WriteInteger(m_writeBuffer, m_protocolID);
    Net::MessageType messageType = message->getType();
    m_writeStream->SerializeByte(messageType);
    message->serialize(*m_writeStream.get());
    
    const int bytesWritten = m_writeStream->GetBitsProcessed() / 8 + (m_writeStream->GetBitsProcessed() % 8 ? 1 : 0);
    writeHeaderData(bytesWritten);
    
    m_socket->Send(address, m_writeBuffer, bytesWritten);
    m_writeStream->Clear();
    memset(m_writeBuffer, 0, bytesWritten);
}

void MasterServer::sendPing(const Net::Address& address, const bool isPong)
{
    uint32_t discard = 0;
    m_writeStream->SerializeInteger(discard);
    Net::Serialization::WriteInteger(m_writeBuffer, m_protocolID);
    Net::MessageType messageType = 0;
    m_writeStream->SerializeByte(messageType);

    std::string PING_STRING = isPong ? "pong" : "ping";
    m_writeStream->SerializeString(PING_STRING);
    const int bytesWritten = m_writeStream->GetBitsProcessed() / 8 + (m_writeStream->GetBitsProcessed() % 8 ? 1 : 0);
    writeHeaderData(bytesWritten);
    
    m_socket->Send(address, m_writeBuffer, bytesWritten);
    m_writeStream->Clear();
    memset(m_writeBuffer, 0, bytesWritten);
}

void MasterServer::onGameHostUpdate(const Net::Address& sender)
{
    std::shared_ptr<GameHostStatusUpdateMessage> message = std::make_shared<GameHostStatusUpdateMessage>();
    message->serialize(*m_readStream.get());

    auto it = std::find_if(m_connectedHosts.begin(),
                           m_connectedHosts.end(), [sender](const GameHostMasterData& data){
        return data.address == sender;
    });
    if (it != m_connectedHosts.end())
    {
        GameHostMasterData& data = *it;
        data.state = message->state;
        data.gameClientPort = message->gameClientPort;
        data.totalCapacity = message->totalCapacity;
        data.currentPlayerCount = message->currentPlayerCount;
        data.allowsJoining = message->allowsJoining;
        data.timeSinceUpdateReceived = 0.f;
        onInfo("MasterServer: received update from host " + sender.GetString());
    }
    else
    {
        GameHostMasterData newGameHost;
        newGameHost.address = sender;
        newGameHost.state = message->state;
        newGameHost.gameClientPort = message->gameClientPort;
        newGameHost.totalCapacity = message->totalCapacity;
        newGameHost.currentPlayerCount = message->currentPlayerCount;
        newGameHost.allowsJoining = message->allowsJoining;
        newGameHost.timeSinceUpdateReceived = 0.f;
        newGameHost.timeSinceUpdateSent = 0.f;
        m_connectedHosts.push_back(newGameHost);
        
        if (m_onConnectedCallback)
        {
            onInfo("MasterServer: game host connected " + sender.GetString());
            m_onConnectedCallback(sender);
        }
    }
}

void MasterServer::onGameListRequest(const Net::Address& sender)
{
    std::shared_ptr<GameListRequestMessage> message = std::make_shared<GameListRequestMessage>();
    message->serialize(*m_readStream.get());

    auto it = std::find_if(m_connectedClients.begin(),
                           m_connectedClients.end(), [sender](const GameClientData& data){
        return data.address == sender;
    });
    if (it != m_connectedClients.end())
    {
        GameClientData& data = *it;
        data.timeSinceUpdate = 0.f;
    }
    else
    {
        GameClientData newGameClient;
        newGameClient.address = sender;
        newGameClient.clientPort = 0;
        newGameClient.timeSinceUpdate = 0.f;
        m_connectedClients.push_back(newGameClient);
    }
//    const uint32_t numConnectedHosts = (uint32_t)m_connectedHosts.size();
//    const uint32_t itemsRequestedCount = message->endRange - message->startRange;
//    const uint32_t itemsResponseCount = std::min(itemsRequestedCount, numConnectedHosts);
//    const uint32_t startRange = std::min(message->startRange, message->endRange - itemsResponseCount);
    
    // Ignoring requested ranges and sending everything for now
    // TODO: Improve this

    std::shared_ptr<GameListResponseMessage> response = std::make_shared<GameListResponseMessage>();
    response->startRange = 0;
    response->endRange = (uint32_t)m_connectedHosts.size();
    response->totalHosts = (uint32_t)m_connectedHosts.size();
    for (const auto& host : m_connectedHosts)
    {
        response->hosts.push_back({host.address, host.name, host.totalCapacity, host.currentPlayerCount});
    }
    sendMessage(response, sender);
    
    onInfo("MasterServer: host list requested by " + sender.GetString() + " reply: " + std::to_string(m_connectedHosts.size()));
}

void MasterServer::onGameConnectionRequest(const Net::Address& sender)
{
    std::shared_ptr<GameClientConnectionRequestMessage> message = std::make_shared<GameClientConnectionRequestMessage>();
    message->serialize(*m_readStream.get());
    
    auto clientIt = std::find_if(m_connectedClients.begin(),
                           m_connectedClients.end(), [sender](const GameClientData& data){
        return data.address == sender;
    });
    if (clientIt == m_connectedClients.end())
    {
        onInfo("MasterServer: unknown client " + sender.GetString() + " requested link to: " + message->hostAddress.GetString());
        return; // Client unknown, ignored
    }
    
    GameClientData& clientData = *clientIt;
    clientData.timeSinceUpdate = 0.f;
    clientData.clientPort = message->clientPort;
    
    auto hostIt = std::find_if(m_connectedHosts.begin(),
                               m_connectedHosts.end(), [message](const GameHostMasterData& data){
        return data.address == message->hostAddress;
    });
    
    const bool hostFound = hostIt != m_connectedHosts.end();
    std::shared_ptr<GameClientConnectionResponseMessage> response = std::make_shared<GameClientConnectionResponseMessage>();

    if (hostFound)
    {
        const auto& hostData = *hostIt;
        std::shared_ptr<GameHostConnectionResponseMessage> hostResponse = std::make_shared<GameHostConnectionResponseMessage>();
        hostResponse->clientAddress = Net::Address(clientData.address.GetAddress(), clientData.clientPort);
        hostResponse->success = true;
        sendMessage(hostResponse, hostData.address);
        response->hostAddress = Net::Address(hostData.address.GetAddress(), hostData.gameClientPort);
        onInfo("MasterServer: establishing link between host " + hostData.address.GetString() + " and client: " + clientData.address.GetString());
    }
    
    response->success = hostFound;
    sendMessage(response, clientData.address);
}

void MasterServer::onInfo(const std::string& info)
{
    if (m_infoCallback)
    {
        m_infoCallback(info);
    }
}
}
