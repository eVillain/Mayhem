#include "DrudgeNet.h"

#include "TransportLAN.h"
#include "TransportIP.h"

#include "WriteStream.h"
#include "ReadStream.h"
#include "MeasureStream.h"
#include "FragmentBuffer.h"
#include "DataConstants.h"
#include "Message.h"
#include "MessageQueue.h"
#include "ReliabilitySystem.h"
#include "MessageFactory.h"
#include <chrono>

namespace Net
{
    DrudgeNet::DrudgeNet(std::shared_ptr<MessageFactory>& messageFactory)
    : m_messageFactory(messageFactory)
    , m_transport(nullptr)
    , m_readStream(nullptr)
    , m_writeStream(nullptr)
    , m_measureStream(nullptr)
    , m_readFragmentBuffer(nullptr)
    , m_writeFragmentBuffer(nullptr)
    , m_readBuffer(nullptr)
    , m_writeBuffer(nullptr)
    , m_packetDataBuffer(nullptr)
    , m_fragmentSenderID(0)
    , m_isConnected(false)
    , m_onConnectedCallback(nullptr)
    , m_onDisconnectedCallback(nullptr)
    , m_onNodeConnectedCallback(nullptr)
    , m_onNodeDisconnectedCallback(nullptr)
    , m_messageReceivedCallback(nullptr)
    , m_onReliableMessageAckedCallback(nullptr)
    {
        m_readBuffer = new unsigned char[BUFFER_SIZE_BYTES];
        m_writeBuffer = new unsigned char[BUFFER_SIZE_BYTES];
        m_packetDataBuffer = new unsigned char[BUFFER_SIZE_BYTES];
        m_readStream = std::make_shared<Net::ReadStream>(m_readBuffer, BUFFER_SIZE_BYTES);
        m_writeStream = std::make_shared<Net::WriteStream>(m_writeBuffer, BUFFER_SIZE_BYTES);
        m_measureStream = std::make_shared<Net::MeasureStream>(BUFFER_SIZE_BYTES);
        m_readFragmentBuffer = std::make_shared<Net::FragmentBuffer>(MAXIMUM_TRANSMISSION_UNIT_BYTES - 20);
        m_writeFragmentBuffer = std::make_shared<Net::FragmentBuffer>(MAXIMUM_TRANSMISSION_UNIT_BYTES - 20);
    }
    
    DrudgeNet::~DrudgeNet()
    {
        delete [] m_readBuffer;
        delete [] m_writeBuffer;
        delete [] m_packetDataBuffer;
    }
    
    void DrudgeNet::initialize(const TransportType transportType,
                               const Address masterServerAddress,
                               const Port masterServerConnectPort,
                               const Port meshPort,
                               const Port serverPort,
                               const Port clientPort,
                               const ProtocolID protocolID,
                               const float meshSendRate,
                               const float timeout,
                               const int32_t maxNodes)
    {
        assert(!m_transport);

        if (transportType == TransportType::Transport_LAN)
        {
            auto transportLAN = std::make_shared<Net::TransportLAN>();
            Net::TransportLAN::Config config;
            config.meshPort = meshPort;
            config.serverPort = serverPort;
            config.clientPort = clientPort;
            config.protocolId = protocolID;
            config.meshSendRate = meshSendRate;
            config.timeout = timeout;
            config.maxNodes = maxNodes;
            transportLAN->Configure(config);
            m_transport = transportLAN;
        }
        else if (transportType == TransportType::Transport_IP)
        {
            auto transportIP = std::make_shared<Net::TransportIP>();
            Net::TransportIP::Config config;
            config.masterServerAddress = masterServerAddress;
            config.masterServerConnectPort = masterServerConnectPort;
            config.meshPort = meshPort;
            config.serverPort = serverPort;
            config.clientPort = clientPort;
            config.protocolID = protocolID;
            config.meshSendRate = meshSendRate;
            config.timeout = timeout;
            config.maxNodes = maxNodes;
            transportIP->Configure(config);
            transportIP->setMasterServerConnectionCallback(std::bind(&DrudgeNet::onMasterServerConnection, this, std::placeholders::_1));
            m_transport = transportIP;
        }
        m_transport->setConnectedCallback(std::bind(&DrudgeNet::onConnected, this));
        m_transport->setDisconnectedCallback(std::bind(&DrudgeNet::onDisconnected, this));
        m_transport->setNodeConnectedCallback(std::bind(&DrudgeNet::onNodeConnected, this, std::placeholders::_1));
        m_transport->setNodeDisconnectedCallback(std::bind(&DrudgeNet::onNodeDisconnected, this, std::placeholders::_1));
        m_transport->setPacketAckedCallback(std::bind(&DrudgeNet::onPacketAcked, this, std::placeholders::_1, std::placeholders::_2));
    }
    
    void DrudgeNet::host(const std::string& name)
    {
        assert(m_transport);
        m_transport->StartServer(name);
    }
    
    void DrudgeNet::join(const std::string& host)
    {
        assert(m_transport);
        m_transport->ConnectClient(host);
    }

    void DrudgeNet::enterLobby()
    {
        assert(m_transport);
        m_transport->EnterLobby();
    }
    
    void DrudgeNet::stop()
    {
        assert(m_transport);
        m_transport->Stop();
    }
    
    MessageID DrudgeNet::sendMessage(const NodeID nodeID,
                                std::shared_ptr<Message>& message,
                                bool reliable /* = false */)
    {
        assert(m_measureStream->GetBitsProcessed() == 0);
        message->serialize(*m_measureStream.get());
        const uint32_t bitsRequired = m_measureStream->GetBitsProcessed();
        m_measureStream->Clear();
        
        if (bitsRequired > BUFFER_SIZE_BYTES * 8)
        {
            return 0;
        }

        std::shared_ptr<MessageQueue> queue;
        if (reliable)
        {
            if (!m_messagesReliable.count(nodeID))
            {
                m_messagesReliable[nodeID] = std::make_shared<MessageQueue>(MessageQueue::Mode::RELIABLE);
            }
            else if (!m_messagesReliable.at(nodeID)->getMessages().empty())
            {
                return 0;
            }
            queue = m_messagesReliable[nodeID];
        }
        else
        {
            if (!m_messagesUnreliable.count(nodeID))
            {
                m_messagesUnreliable[nodeID] = std::make_shared<MessageQueue>(MessageQueue::Mode::UNRELIABLE);
            }
            queue = m_messagesUnreliable[nodeID];
        }
        
        MessageID messageID = queue->queueMessage(message, bitsRequired);
        return messageID;
    }
    
    void DrudgeNet::update(const float deltaTime)
    {
        m_transport->Update(deltaTime);
        
        if (m_transport->IsConnected())
        {
            if (!m_isConnected)
            {
                m_isConnected = true;

                onNodeConnected(0);
            }
        }
        else
        {
            if (m_isConnected)
            {
                m_isConnected = false;

                onNodeDisconnected(0);
            }
        }
    }
    
    void DrudgeNet::receiveMessages()
    {
        assert(m_readStream->GetBitsProcessed() == 0);

        while (true)
        {
            NodeID nodeID = -1;
            int receivedBytes = m_transport->ReceivePacket(nodeID, m_readBuffer, BUFFER_SIZE_BYTES);
            if (receivedBytes == 0)
                break;
            
            uint32_t packetType = PacketType::Packet_Single;
            m_readStream->SerializeBits(packetType, Stream::BitsRequired(PacketType::PacketType_NumTypes));
            PadDataToNearestByte(m_readStream);

            if (packetType == PacketType::Packet_Fragment)
            {
                processReadStreamFragment(nodeID, receivedBytes);
            }
            else
            {
                processReadStream(nodeID, receivedBytes);
            }
            
            memset(m_readBuffer, 0, receivedBytes);
            m_readStream->Clear();
        }
    }
    
    void DrudgeNet::processReadStream(const NodeID senderNode,
                                      const int receivedBytes)
    {
        int processedBytes = 0;
        while (processedBytes < receivedBytes)
        {
            printf("DrudgeNet::processReadStream received %i bytes, processed %i from: %i\n", receivedBytes, processedBytes, senderNode);

            std::shared_ptr<Message> message = m_messageFactory->create(*m_readStream.get());
            if (!message)
            {
                printf("DrudgeNet::processReadStream failed to deserialise message from node %i\n", senderNode);
                break;
            }
            
            if (m_messageReceivedCallback)
            {
                m_messageReceivedCallback(message, senderNode);
            }
            PadDataToNearestByte(m_readStream);
            
            processedBytes = m_readStream->GetBitsProcessed() / 8;
        }
    }
    
    void DrudgeNet::processReadStreamFragment(const NodeID senderNode, const int receivedBytes)
    {
        PacketFragmentID fragmentID = 0;
        uint8_t fragmentCount = 0;
        m_readStream->SerializeByte(fragmentID);
        m_readStream->SerializeByte(fragmentCount);

        const int dataOffset = m_readStream->GetBitsProcessed() / 8;
        const int fragSize = receivedBytes - dataOffset;
        if (m_readFragmentBuffer->getData() == nullptr)
        {
            m_fragmentSenderID = senderNode;
            m_readFragmentBuffer->setupFragmentData(nullptr, fragmentCount * 1180);
        }
        else if (m_fragmentSenderID != senderNode)
        {
            memset(m_readBuffer, 0, receivedBytes);
            m_readStream->Clear();
            return;
        }
        m_readFragmentBuffer->onFragmentReceived(&m_readBuffer[dataOffset], fragSize, fragmentID, fragmentCount);

        if (m_readFragmentBuffer->isComplete())
        {
            int32_t packetBytes = 0;
            for (PacketFragmentID packetID = 0; packetID < m_readFragmentBuffer->getFragmentCount(); packetID++)
            {
                packetBytes += m_readFragmentBuffer->getFragmentSize(packetID);
            }

            m_readStream->Clear();
            memcpy(m_readBuffer, m_readFragmentBuffer->getData(), packetBytes);
            m_readFragmentBuffer->clearData();
            
            processReadStream(senderNode, packetBytes);
        }
    }

    void DrudgeNet::sendMessages()
    {
        assert(m_writeStream->GetBitsProcessed() == 0);
        
        SendMessages(m_messagesUnreliable, false);

        SendMessages(m_messagesReliable, true);
    }
    
    void DrudgeNet::SendMessages(std::map<NodeID, std::shared_ptr<MessageQueue>> queue, bool reliable)
    {
        assert(m_writeStream->GetBitsProcessed() == 0);

        memset(m_packetDataBuffer, 0, BUFFER_SIZE_BYTES);

        for (auto nodeId2messageQueue : queue)
        {
            const NodeID nodeID = nodeId2messageQueue.first;
            auto messageQueue = nodeId2messageQueue.second;
            const size_t messageCount = messageQueue->getMessages().size();

            if (!m_transport->IsNodeConnected(nodeID))
            {
                messageQueue->clear();
                continue;
            }
            
            if (messageCount == 0)
            {
                continue;
            }
            
            int32_t bitsAvailable = BUFFER_SIZE_BYTES;
            MessageID messageID = 0;
            MessageQueue::MessageData messageData;
            
            for (size_t i = 0; i < messageCount; i++)
            {
                if (messageQueue->getNextMessage(bitsAvailable, messageID, messageData))
                {
                    MessageType messageType = messageData.message->getType();
                    m_writeStream->SerializeByte(messageType);
                    messageData.message->serialize(*m_writeStream.get());
                    bitsAvailable -= messageData.bitsRequired;
                    if (!reliable)
                    {
                        messageQueue->removeMessage(messageID);
                    }
                }
                else if (m_writeStream->GetBitsProcessed())
                {
                    PadDataToNearestByte(m_writeStream);
                    const int bytesProcessed = m_writeStream->GetBitsProcessed() / 8;
                    memcpy(m_packetDataBuffer, m_writeBuffer, bytesProcessed);
                    memset(m_writeBuffer, 0, bytesProcessed);
                    m_writeStream->Clear();
                    SendPacket(nodeID, m_packetDataBuffer, bytesProcessed);
                    
                    if (reliable)
                    {
                        uint32_t packetSequence = m_transport->GetReliability(nodeID)->GetLocalSequence() - 1;
                        m_sentReliablePackets[nodeID].push_back(packetSequence);
                    }
                }
            }
            
            if (m_writeStream->GetBitsProcessed())
            {
                PadDataToNearestByte(m_writeStream);
                const int bytesProcessed = m_writeStream->GetBitsProcessed() / 8;
                memcpy(m_packetDataBuffer, m_writeBuffer, bytesProcessed);
                memset(m_writeBuffer, 0, bytesProcessed);
                m_writeStream->Clear();
                SendPacket(nodeID, m_packetDataBuffer, bytesProcessed);
                
                if (reliable)
                {
                    uint32_t packetSequence = m_transport->GetReliability(nodeID)->GetLocalSequence() -1;
                    m_sentReliablePackets[nodeID].push_back(packetSequence);
                }
            }
        }
    }

    bool DrudgeNet::SendPacket(const NodeID nodeID,
                               const unsigned char data[],
                               const int32_t size)
    {
        const int32_t fragmentSize = MAXIMUM_TRANSMISSION_UNIT_BYTES - 20;
        
        if (size >= fragmentSize)
        {
            if (m_writeFragmentBuffer->getDataSize() != 0)
            {
                printf("DrudgeNet::SendPacket Error! Already sending a fragmented packet!\n");
                return false;
            }
            m_writeFragmentBuffer->setupFragmentData(data, size);
            const int32_t fragmentCount = m_writeFragmentBuffer->getFragmentCount();
            for (PacketFragmentID fragmentID = 0; fragmentID < fragmentCount; fragmentID++)
            {
                if (!SendFragment(nodeID,
                                  m_writeFragmentBuffer->getFragmentData(fragmentID),
                                  m_writeFragmentBuffer->getFragmentSize(fragmentID),
                                  fragmentID, fragmentCount))
                {
                    printf("DrudgeNet::SendPacket Error! Failed to send fragment!\n");
                    m_writeFragmentBuffer->clearData();
                    return false;
                }
            }
            m_writeFragmentBuffer->clearData();
            return true;
        }
        else
        {
            uint32_t packet_type = PacketType::Packet_Single;
            m_writeStream->SerializeBits(packet_type, Stream::BitsRequired(PacketType::PacketType_NumTypes));
            PadDataToNearestByte(m_writeStream);
            
            const int32_t headerBytes = m_writeStream->GetBitsProcessed() / 8;
            memcpy(m_writeBuffer + headerBytes, data, size);
            
            bool success = m_transport->SendPacket(nodeID, m_writeBuffer, size + headerBytes);
            if (!success)
            {
                printf("DrudgeNet::SendPacket failed at Transport level\n");
            }
            memset(m_writeBuffer, 0, size + headerBytes);
            m_writeStream->Clear();
            
            return success;
        }
    }
    
    bool DrudgeNet::SendFragment(const NodeID nodeID,
                                 const unsigned char data[],
                                 const int32_t size,
                                 PacketFragmentID fragmentID,
                                 uint8_t fragmentCount)
    {
        uint32_t packet_type = PacketType::Packet_Fragment;
        m_writeStream->SerializeBits(packet_type, Stream::BitsRequired(PacketType::PacketType_NumTypes));
        PadDataToNearestByte(m_writeStream);
        m_writeStream->SerializeByte(fragmentID);
        m_writeStream->SerializeByte(fragmentCount);

        const int32_t headerBytes = m_writeStream->GetBitsProcessed() / 8;
        memcpy(m_writeBuffer + headerBytes, data, size);
        
        bool success = m_transport->SendPacket(nodeID, m_writeBuffer, size + headerBytes);

        memset(m_writeBuffer, 0, size + headerBytes);
        m_writeStream->Clear();
        
        return success;
    }
    
    void DrudgeNet::onPacketAcked(PacketSequenceID ackedPacketSequence, NodeID nodeID)
    {
        if (m_messagesReliable.count(nodeID))
        {
            bool found = false;
            auto it = m_sentReliablePackets.find(nodeID);
            if (it != m_sentReliablePackets.end())
            {
                const std::vector<PacketSequenceID>& packets = it->second;
                for (const PacketSequenceID packetID : packets)
                {
                    if (packetID == ackedPacketSequence)
                    {
                        found = true;
                        break;
                    }
                }
            }
            if (found)
            {
                if (m_onReliableMessageAckedCallback)
                {
                    const auto& messages = m_messagesReliable.at(nodeID)->getMessages();
                    const MessageID messageID = messages.begin()->first;
                    m_onReliableMessageAckedCallback(nodeID, messageID);
                }
                m_sentReliablePackets.erase(it);
                m_messagesReliable.erase(nodeID);
            }
        }
        
        if (m_writeFragmentBuffer->getFragmentCount())
        {
            m_writeFragmentBuffer->onPacketAcked(ackedPacketSequence);
        }
    }

    void DrudgeNet::onConnected()
    {
        if (m_onConnectedCallback)
        {
            m_onConnectedCallback();
        }
    }

    void DrudgeNet::onDisconnected()
    {
        m_isConnected = false;

        if (m_onDisconnectedCallback)
        {
            m_onDisconnectedCallback();
        }
    }

    void DrudgeNet::onNodeConnected(const NodeID nodeID)
    {
        if (m_onNodeConnectedCallback)
        {
            m_onNodeConnectedCallback(nodeID);
        }
    }

    void DrudgeNet::onNodeDisconnected(const NodeID nodeID)
    {
        auto reliableIt = m_messagesReliable.find(nodeID);
        if (reliableIt != m_messagesReliable.end())
        {
            m_messagesReliable[nodeID]->clear();
        }
        auto unreliableIt = m_messagesUnreliable.find(nodeID);
        if (unreliableIt != m_messagesUnreliable.end())
        {
            m_messagesUnreliable[nodeID]->clear();
        }
        auto sentIt = m_sentReliablePackets.find(nodeID);
        if (sentIt != m_sentReliablePackets.end())
        {
            m_sentReliablePackets[nodeID].clear();
        }

        if (m_onNodeDisconnectedCallback)
        {
            m_onNodeDisconnectedCallback(nodeID);
        }
    }

    void DrudgeNet::onMasterServerConnection(bool connected)
    {
        if (m_masterServerConnectionCallback)
        {
            m_masterServerConnectionCallback(connected);
        }
    }

    void DrudgeNet::PadDataToNearestByte(std::shared_ptr<Net::Stream> stream)
    {
        if (const uint32_t overByteBoundaryBits = stream->GetBitsProcessed() % 8)
        {
            const uint32_t paddingBitsRequired = 8 - overByteBoundaryBits;
            bool PADDING = false;
            for (uint32_t i = 0; i < paddingBitsRequired; i++)
            {
                stream->SerializeBoolean(PADDING);
            }
        }
    }

    const uint64_t DrudgeNet::getTimeStamp()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
}

