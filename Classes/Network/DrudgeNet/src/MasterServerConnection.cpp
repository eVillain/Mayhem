#include "MasterServerConnection.h"

#include "CRC32.h"
#include "DataConstants.h"
#include "MasterServerMessages.h"
#include "ReadStream.h"
#include "Serialization.h"
#include "Socket.h"
#include "WriteStream.h"
#include <iostream>

namespace Net {
MasterServerConnection::MasterServerConnection(const Mode mode,
                                               const Net::Port serverPort,
                                               const Net::ProtocolID protocolID,
                                               const float timeOut)
: m_socket(nullptr)
, m_readStream(nullptr)
, m_writeStream(nullptr)
, m_readBuffer(nullptr)
, m_writeBuffer(nullptr)
, m_infoCallback(nullptr)
, m_mode(mode)
, m_state(State::DISCONNECTED)
, m_port(serverPort)
, m_protocolID(protocolID)
, m_timeout(timeOut)
, m_timeSinceUpdateReceived(0.f)
, m_timeSinceUpdateSent(0.f)
{
    m_socket = std::unique_ptr<Net::Socket>(new Net::Socket());
    m_readBuffer = new unsigned char[Net::BUFFER_SIZE_BYTES];
    m_writeBuffer = new unsigned char[Net::BUFFER_SIZE_BYTES];
    memset(m_readBuffer, 0, Net::BUFFER_SIZE_BYTES);
    memset(m_writeBuffer, 0, Net::BUFFER_SIZE_BYTES);
    m_readStream = std::unique_ptr<Net::ReadStream>(new Net::ReadStream(m_readBuffer, Net::BUFFER_SIZE_BYTES));
    m_writeStream = std::unique_ptr<Net::WriteStream>(new Net::WriteStream(m_writeBuffer, Net::BUFFER_SIZE_BYTES));
}

MasterServerConnection::~MasterServerConnection()
{
    delete [] m_readBuffer;
    delete [] m_writeBuffer;
}

bool MasterServerConnection::connect(const Net::Address& address)
{
    if (m_state != State::DISCONNECTED)
    {
        return false;
    }
    
    onInfo("MasterServerConnection: connecting to: " + address.GetString());
    
    if (!m_socket->Open(m_port))
    {
        onInfo("MasterServerConnection: failed to open socket on port: " + std::to_string(m_port));
        return false;
    }
    
    m_serverAddress = address;
    m_state = State::CONNECTING;

    return true;
}

void MasterServerConnection::disconnect()
{
    onInfo("MasterServerConnection: disconnecting");
    m_socket->Close();
    m_state = State::DISCONNECTED;
}

void MasterServerConnection::update(const float deltaTime)
{
    if (m_state == State::DISCONNECTED ||
        m_state == State::CONNECT_FAILED)
    {
        return;
    }
    
    m_timeSinceUpdateReceived += deltaTime;
    m_timeSinceUpdateSent += deltaTime;
    
    readSocketData();
    
    updateConnection(deltaTime);
}

void MasterServerConnection::requestGameList(const uint32_t startRange,
                                             const uint32_t endRange)
{
    onInfo("MasterServerConnection: requesting game list");
    std::shared_ptr<GameListRequestMessage> message = std::make_shared<GameListRequestMessage>();
    message->startRange = 0;
    message->endRange = 100;
    sendMessage(message);
}

void MasterServerConnection::requestLinkToHost(const Net::Address& hostAddress,
                                               const Net::Port clientPort)
{
    onInfo("MasterServerConnection: requesting link to host: " + hostAddress.GetString());
    std::shared_ptr<GameClientConnectionRequestMessage> message = std::make_shared<GameClientConnectionRequestMessage>();
    message->hostAddress = hostAddress;
    message->clientPort = clientPort;
    sendMessage(message);
}

void MasterServerConnection::sendHostUpdate(const uint32_t totalCapacity,
                                            const uint32_t currentPlayerCount,
                                            const GameHostState state,
                                            const Net::Port gameClientPort,
                                            const bool allowsJoining)
{
    onInfo("MasterServerConnection: sending host state update: ");
    std::shared_ptr<GameHostStatusUpdateMessage> message = std::make_shared<GameHostStatusUpdateMessage>();
    message->totalCapacity = totalCapacity;
    message->currentPlayerCount = currentPlayerCount;
    message->state = state;
    message->gameClientPort = gameClientPort;
    message->allowsJoining = allowsJoining;
    sendMessage(message);
}

void MasterServerConnection::readSocketData()
{
    while (true)
    {
        Net::Address sender;
        const int receivedBytes = m_socket->Receive(sender, m_readBuffer, Net::BUFFER_SIZE_BYTES);
        if (receivedBytes == 0)
        {
            break;
        }
        processReceivedData(sender, receivedBytes);
    }
}

void MasterServerConnection::processReceivedData(const Net::Address& sender,
                                                 const int receivedBytes)
{
    if (sender != m_serverAddress)
    {
        onInfo("MasterServerConnection: WARNING: data from unknown address " + sender.GetString());
        return;
    }
    if (!validateDataHeader(receivedBytes))
    {
        onInfo("MasterServerConnection: WARNING: received invalid header from " + sender.GetString());
        memset(m_readBuffer, 0, receivedBytes);
        m_readStream->Clear();
        return;
    }
    
    if (m_state == State::CONNECTING)
    {
        m_state = State::CONNECTED;
        if (m_connectCallback)
        {
            m_connectCallback();
        }
    }
    
//    std::cout << "MasterServerConnection read: ";
//    for (int i = 0; i < receivedBytes; i++)
//    {
//        std::cout << std::to_string(m_readBuffer[i]) << ", ";
//    }
//    std::cout << "\n";

    m_timeSinceUpdateReceived = 0.f;
    
    Net::MessageType messageType = 0;
    m_readStream->SerializeByte(messageType);
    if (messageType == PingMessageType)
    {
        std::string pingy;
        m_readStream->SerializeString(pingy);
        if (pingy != "pong")
        {
            sendPing(true);
        }
    }
    else if (messageType == GameListResponseMessageType)
    {
        if (m_mode == Mode::MODE_GAME_HOST)
        {
            onInfo("MasterServerConnection: WARNING: received game list from server while in host mode");
        }
        else
        {
            onGameListResponse();
        }
    }
    else if (messageType == GameClientConnectionResponseMessageType)
    {
        if (m_mode == Mode::MODE_GAME_HOST)
        {
            onInfo("MasterServerConnection: WARNING: received client connection reponse from server while in host mode");
        }
        else
        {
            onClientConnectionResponse();
        }
    }
    else if (messageType == GameHostConnectionResponseMessageType)
    {
        if (m_mode == Mode::MODE_GAME_HOST)
        {
            onHostConnectionResponse();
        }
        else
        {
            onInfo("MasterServerConnection: WARNING: received host connection reponse from server while in client mode");
        }
    }
    
    memset(m_readBuffer, 0, receivedBytes);
    m_readStream->Clear();
}

void MasterServerConnection::updateConnection(const float deltaTime)
{
    if (m_state == State::CONNECTING || m_state == State::CONNECTED)
    {
        if (m_timeSinceUpdateReceived >= m_timeout)
        {
            if (m_disconnectCallback)
            {
                onInfo("MasterServerConnection: disconnected from master server " + m_serverAddress.GetString());
                m_disconnectCallback();
                return;
            }
        }
        
        const float UPDATE_SEND_RATE = 2.f;
        
        if (m_timeSinceUpdateSent < UPDATE_SEND_RATE)
        {
            return;
        }
        
        sendPing(false);
    }
}

bool MasterServerConnection::validateDataHeader(const int receivedBytes)
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

void MasterServerConnection::writeHeaderData(const int dataBytes)
{
    uint32_t crc = 0;
    crc = crc32c(crc, m_writeBuffer, dataBytes);
    Net::Serialization::WriteInteger(m_writeBuffer, crc);
}

void MasterServerConnection::sendMessage(const std::shared_ptr<Net::Message>& message)
{
    uint32_t discard = 0;
    m_writeStream->SerializeInteger(discard);
    Net::Serialization::WriteInteger(m_writeBuffer, m_protocolID);
    Net::MessageType messageType = message->getType();
    m_writeStream->SerializeByte(messageType);
    message->serialize(*m_writeStream.get());
    const int bytesWritten = m_writeStream->GetBitsProcessed() / 8 + (m_writeStream->GetBitsProcessed() % 8 ? 1 : 0);
    writeHeaderData(bytesWritten);

    m_socket->Send(m_serverAddress, m_writeBuffer, bytesWritten);
    m_writeStream->Clear();
    memset(m_writeBuffer, 0, bytesWritten);
    m_timeSinceUpdateSent = 0.f;
}

void MasterServerConnection::sendPing(const bool isPong)
{
    std::string PING_STRING = isPong ? "pong" : "ping";
    Net::MessageType messageType = 0;
    uint32_t discard = 0;
    m_writeStream->SerializeInteger(discard);
    Net::Serialization::WriteInteger(m_writeBuffer, m_protocolID);
    m_writeStream->SerializeByte(messageType);
    m_writeStream->SerializeString(PING_STRING);
    const int bytesWritten = m_writeStream->GetBitsProcessed() / 8 + (m_writeStream->GetBitsProcessed() % 8 ? 1 : 0);
    writeHeaderData(bytesWritten);
    
    m_socket->Send(m_serverAddress, m_writeBuffer, bytesWritten);
    m_writeStream->Clear();
    memset(m_writeBuffer, 0, bytesWritten);
    m_timeSinceUpdateSent = 0.f;
}

void MasterServerConnection::onGameListResponse()
{
    std::shared_ptr<GameListResponseMessage> message = std::make_shared<GameListResponseMessage>();
    message->serialize(*m_readStream.get());
    if (m_hostListCallback)
    {
        m_hostListCallback(message->hosts, message->startRange, message->endRange);
    }
    onInfo("MasterServerConnection: received game host list of size " + std::to_string(message->hosts.size()));
}

void MasterServerConnection::onClientConnectionResponse()
{
    std::shared_ptr<GameClientConnectionResponseMessage> message = std::make_shared<GameClientConnectionResponseMessage>();
    message->serialize(*m_readStream.get());
    if (m_clientConnectCallback)
    {
        m_clientConnectCallback(message->hostAddress, message->success);
    }
    if (message->success)
    {
        onInfo("MasterServerConnection: received client connection success to host " + message->hostAddress.GetString());
    }
    else
    {
        onInfo("MasterServerConnection: received client connection fail to host " + message->hostAddress.GetString());
    }
}

void MasterServerConnection::onHostConnectionResponse()
{
    std::shared_ptr<GameHostConnectionResponseMessage> message = std::make_shared<GameHostConnectionResponseMessage>();
    message->serialize(*m_readStream.get());
    if (m_hostConnectCallback)
    {
        m_hostConnectCallback(message->clientAddress, message->success);
    }
    if (message->success)
    {
        onInfo("MasterServerConnection: received host connection success to client " + message->clientAddress.GetString());
    }
    else
    {
        onInfo("MasterServerConnection: received host connection fail to client " + message->clientAddress.GetString());
    }
}

void MasterServerConnection::onInfo(const std::string& info)
{
    if (m_infoCallback)
    {
        m_infoCallback(info);
    }
}
}
