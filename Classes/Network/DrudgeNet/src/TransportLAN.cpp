#include "TransportLAN.h"

#include "Address.h"
#include "Socket.h"
#include "Beacon.h"
#include "Listener.h"
#include "Connection.h"
#include "ReliabilitySystem.h"
#include "Serialization.h"
#include "Mesh.h"
#include "Node.h"
#include "WriteStream.h"
#include "ReadStream.h"
#include "DataConstants.h"
#include "FlowControl.h"

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <cassert>

#ifdef DEBUG
#define NET_UNIT_TEST
#endif

namespace Net
{
    TransportLAN::TransportLAN()
    : mesh(nullptr)
    , node(nullptr)
    , beacon(nullptr)
    , listener(nullptr)
    , m_readStream(nullptr)
    , m_writeStream(nullptr)
    , m_readBuffer(nullptr)
    , m_writeBuffer(nullptr)
    , m_tickAccumulator(-1)
    , m_connectAccumulator(0.0f)
    , m_connectingByName(false)
    , m_connectFailed(false)
    , m_packetCallback(nullptr)
    , m_onConnectedCallback(nullptr)
    , m_onDisconnectedCallback(nullptr)
    , m_onNodeConnectedCallback(nullptr)
    , m_onNodeDisconnectedCallback(nullptr)
    {
        m_readBuffer = new unsigned char[BUFFER_SIZE_BYTES];
        m_writeBuffer = new unsigned char[BUFFER_SIZE_BYTES];
        
        m_readStream = std::make_shared<Net::ReadStream>(m_readBuffer, BUFFER_SIZE_BYTES);
        m_writeStream = std::make_shared<Net::WriteStream>(m_writeBuffer, BUFFER_SIZE_BYTES);
    }
    
    TransportLAN::~TransportLAN()
    {
        Stop();
    }
    
    void TransportLAN::Configure(Config& config)
    {
        assert(!node);
        assert(!mesh);
        assert(!beacon);
        assert(!listener);
        m_config = config;
    }
    
    const TransportLAN::Config& TransportLAN::GetConfig() const
    {
        return m_config;
    }
    
    bool TransportLAN::StartServer(const std::string& name)
    {
        assert(!node);
        assert(!mesh);
        assert(!beacon);
        assert(!listener);
        
        beacon = new Beacon(name, m_config.protocolId, m_config.listenerPort, m_config.meshPort);
        if (!beacon->Start(m_config.beaconPort))
        {
            printf("LAN Transport:failed to start beacon on port %d\n", m_config.beaconPort);
            Stop();
            return false;
        }
        
        mesh = new Mesh(m_config.protocolId, m_config.maxNodes, m_config.meshSendRate, m_config.timeout);
        if (!mesh->Start(m_config.meshPort))
        {
            printf("LAN Transport:failed to start mesh on port %d\n", m_config.meshPort);
            Stop();
            return 1;
        }
        
        node = new Node(m_config.protocolId, m_config.meshSendRate, m_config.timeout, MAXIMUM_TRANSMISSION_UNIT_BYTES);
        if (!node->Start(m_config.serverPort))
        {
            printf("LAN Transport:failed to start node on port %d\n", m_config.serverPort);
            Stop();
            return 1;
        }
        setupNodeCallbacks();
        
        mesh->Reserve(0, Address(127, 0, 0, 1, m_config.serverPort));
        node->Join(Address(127, 0, 0, 1, m_config.meshPort));

        return true;
    }
    
    bool TransportLAN::ConnectClient(const std::string& server)
    {
        assert(!node);
        assert(!mesh);
        assert(!beacon);
        assert(!listener);
        
        // connect by address?
        uint32_t  a = 0;
        uint32_t  b = 0;
        uint32_t  c = 0;
        uint32_t  d = 0;
        uint32_t  port = 0;
        bool isAddress = false;
        if (sscanf(server.c_str(), "%d.%d.%d.%d:%d", &a, &b, &c, &d, &port))
        {
            isAddress = true;
        }
        else
        {
            port = m_config.meshPort;
            if (sscanf(server.c_str(), "%d.%d.%d.%d", &a, &b, &c, &d))
            {
                isAddress = true;
            }
        }
        
        if (isAddress)
        {
            printf("LAN Transport: client connect to address: %d.%d.%d.%d:%d\n", a, b, c, d, port);
            node = new Node(m_config.protocolId, m_config.meshSendRate, m_config.timeout, MAXIMUM_TRANSMISSION_UNIT_BYTES);
            if (!node->Start( m_config.clientPort))
            {
                printf( "LAN Transport: failed to start node on port %d\n", m_config.serverPort );
                Stop();
                return 1;
            }
            setupNodeCallbacks();
            
            node->Join(Address((unsigned char)a,
                               (unsigned char)b,
                               (unsigned char)c,
                               (unsigned char)d,
                               (unsigned short)port));
            return true;
        }
        else
        {
            printf("LAN Transport: client connect by name \"%s\"\n", server.c_str());
            listener = new Listener( m_config.protocolId, m_config.timeout );
            if (!listener->Start(m_config.listenerPort))
            {
                printf("LAN Transport: failed to start listener on port %d\n", m_config.listenerPort);
                Stop();
                return false;
            }
            m_connectingByName = true;
            strncpy(connectName, server.c_str(), sizeof(connectName) - 1);
            connectName[sizeof(connectName) - 1] = '\0';
            m_connectAccumulator = 0.0f;
            m_connectFailed = false;
        }
        return true;
    }
    
    bool TransportLAN::IsConnected() const
    {
        return node && node->IsConnected();
    }
    
    bool TransportLAN::ConnectFailed() const
    {
        return (node && node->JoinFailed()) || (m_connectingByName && m_connectFailed);
    }
    
    bool TransportLAN::EnterLobby()
    {
        assert(!listener);
        printf("LAN Transport: enter lobby\n");
        listener = new Listener(m_config.protocolId, m_config.timeout);
        if (!listener->Start(m_config.listenerPort))
        {
            printf("LAN Transport: failed to start listener on port %d\n", m_config.listenerPort);
            Stop();
            return false;
        }
        return true;
    }
    
    int32_t TransportLAN::GetLobbyEntryCount() const
    {
        if (listener)
        {
            return listener->GetEntryCount();
        }
        else
        {
            return 0;
        }
    }
    
    bool TransportLAN::GetLobbyEntryAtIndex(int32_t index, LobbyEntry& entry)
    {
        if (!listener || index < 0 || index >= listener->GetEntryCount())
        {
            return false;
        }

        const ListenerEntry& e = listener->GetEntry(index);
        entry.address = e.address;
        entry.name = e.name;
        entry.maxClients = 0;
        entry.currentClients = 0;
        return true;
    }
    
    void TransportLAN::Stop()
    {
        printf( "LAN Transport: stop\n" );
        if ( mesh )
        {
            delete mesh;
            mesh = NULL;
        }
        if ( node )
        {
            delete node;
            node = NULL;
        }
        if ( beacon )
        {
            delete beacon;
            beacon = NULL;
        }
        if ( listener )
        {
            delete listener;
            listener = NULL;
        }
        m_connectingByName = false;
        m_connectFailed = false;
        
        memset(m_readBuffer, 0, BUFFER_SIZE_BYTES);
        memset(m_writeBuffer, 0, BUFFER_SIZE_BYTES);
        m_readStream->Clear();
        m_writeStream->Clear();
    }
    
    // implement transport interface
    
    bool TransportLAN::IsNodeConnected( NodeID nodeId )
    {
        assert(node);
        return node->IsNodeConnected( nodeId );
    }
    
    NodeID TransportLAN::GetLocalNodeId() const
    {
        assert(node);
        return node->GetLocalNodeId();
    }

    const Address& TransportLAN::GetNodeAddress(NodeID nodeId) const
    {
        assert(node);
        return node->GetNodeAddress(nodeId);
    }
    
    int32_t TransportLAN::GetMaxNodes() const
    {
        assert(node);
        return node->GetMaxNodes();
    }
  
    bool TransportLAN::SendPacket(NodeID nodeID,
                                  const unsigned char data[],
                                  int32_t size)
    {
        assert(node);
        assert(size < BUFFER_SIZE_BYTES);
        assert(m_writeStream->GetBitsProcessed() == 0);
        
        const float sendRate = m_flowControl[nodeID]->GetSendRate();
        if (m_sendAccumulators[nodeID] < 1.0f / sendRate)
        {
            return false;
        }
        
        m_sendAccumulators[nodeID] -= 1.0f / sendRate;
        
        std::shared_ptr<ReliabilitySystem> reliabilitySystem = GetReliability(nodeID);
        uint32_t seq = reliabilitySystem->GetLocalSequence();
        uint32_t ack = reliabilitySystem->GetRemoteSequence();
        uint32_t ack_bits = reliabilitySystem->GenerateAckBits();
        
        m_writeStream->SerializeInteger(seq);
        m_writeStream->SerializeInteger(ack);
        m_writeStream->SerializeInteger(ack_bits);
        
        const int32_t headerBytes = m_writeStream->GetBitsProcessed() / 8;
        memcpy(m_writeBuffer + headerBytes, data, size);
        
        bool success = node->SendPacket(nodeID, m_writeBuffer, size + headerBytes);
        if (success)
        {
            reliabilitySystem->PacketSent(size);
        }
        else
        {
            printf("TransportLAN send failed, node said no\n");
        }
        
        memset(m_writeBuffer, 0, size + headerBytes);
        m_writeStream->Clear();
        
        return success;        
    }
    
    int32_t TransportLAN::ReceivePacket(NodeID& nodeId,
                                        unsigned char data[],
                                        int32_t size)
    {
        assert(node);
        
        int32_t receivedBytes = node->ReceivePacket(nodeId, m_readBuffer, BUFFER_SIZE_BYTES);
        if (receivedBytes == 0)
        {
            return 0;
        }

        uint32_t packet_sequence = 0;
        uint32_t packet_ack = 0;
        uint32_t packet_ack_bits = 0;

        m_readStream->SerializeInteger(packet_sequence);
        m_readStream->SerializeInteger(packet_ack);
        m_readStream->SerializeInteger(packet_ack_bits);

        const int32_t headerBytes = m_readStream->GetBitsProcessed() / 8;
        if (size < receivedBytes - headerBytes)
        {
            memset(m_readBuffer, 0, receivedBytes);
            m_readStream->Clear();
            return 0;
        }
        
        std::shared_ptr<ReliabilitySystem> reliabilitySystem = GetReliability(nodeId);
        reliabilitySystem->PacketReceived(packet_sequence, receivedBytes - headerBytes);
        reliabilitySystem->ProcessAck(packet_ack, packet_ack_bits);

        memcpy(data, m_readBuffer + headerBytes, receivedBytes - headerBytes);
    
        memset(m_readBuffer, 0, receivedBytes);
        m_readStream->Clear();

//        printf("LAN Transport Received %i bytes from node  %i, sequence %i\n", receivedBytes, nodeId, packet_sequence);

        return receivedBytes - headerBytes;
    }
    
    std::shared_ptr<ReliabilitySystem> TransportLAN::GetReliability(NodeID nodeId)
    {
        if (m_reliabilitySystems.find(nodeId) == m_reliabilitySystems.end())
        {
            m_reliabilitySystems[nodeId] = std::make_shared<ReliabilitySystem>();
        }
        return m_reliabilitySystems[nodeId];
    }
    
    void TransportLAN::Update(float deltaTime)
    {
        m_tickAccumulator++;
        
        if (m_connectingByName && !m_connectFailed)
        {
            AttemptConnection(deltaTime);
        }

        if (beacon)
        {
            beacon->Update(deltaTime);
        }
        if (listener)
        {
            listener->Update(deltaTime);
        }
        if (mesh)
        {
            mesh->Update(deltaTime);
            const int maxNodes = GetMaxNodes();
            for (int nodeID = 1; nodeID < maxNodes; nodeID++)
            {
                if (IsNodeConnected(nodeID))
                {
                    m_flowControl[nodeID]->Update(deltaTime, GetReliability(nodeID)->GetRoundTripTime() * 1000.f);
                }
            }
        }
        if (node)
        {
            node->Update(deltaTime);
            
            if (node->IsConnected())
            {
                m_flowControl[0]->Update(deltaTime, GetReliability(0)->GetRoundTripTime() * 1000.f);
            }
        }
        if (mesh || node)
        {
            UpdateReliabilitySystems(deltaTime);
            
            for (auto& accumulator : m_sendAccumulators)
            {
                accumulator.second = accumulator.second + deltaTime;
            }
        }
    }
    
    TransportType TransportLAN::GetType() const
    {
        return Transport_LAN;
    }
    
    void TransportLAN::AttemptConnection(const float deltaTime)
    {
        assert(listener);
        const int32_t entryCount = listener->GetEntryCount();
        for (int32_t i = 0; i < entryCount; ++i)
        {
            const ListenerEntry & entry = listener->GetEntry(i);
            if (strcmp(entry.name, connectName) == 0)
            {
                printf("LAN Transport: found server %d.%d.%d.%d:%d\n",
                       entry.address.GetA(),
                       entry.address.GetB(),
                       entry.address.GetC(),
                       entry.address.GetD(),
                       entry.address.GetPort());
                node = new Node(m_config.protocolId, m_config.meshSendRate, m_config.timeout, MAXIMUM_TRANSMISSION_UNIT_BYTES);
                if (!node->Start(m_config.clientPort))
                {
                    printf("LAN Transport: failed to start node on port %d\n", m_config.serverPort);
                    Stop();
                    m_connectFailed = true;
                    return;
                }
                setupNodeCallbacks();

                node->Join(entry.address);
                delete listener;
                listener = NULL;
                m_connectingByName = false;
            }
        }
        if (m_connectingByName)
        {
            m_connectAccumulator += deltaTime;
            if (m_connectAccumulator > m_config.timeout)
            {
                m_connectFailed = true;
            }
        }
    }
    
    void TransportLAN::UpdateReliabilitySystems(const float deltaTime)
    {
        for (auto& reliabilityPair : m_reliabilitySystems)
        {
            std::shared_ptr<ReliabilitySystem>& reliabilitySystem = reliabilityPair.second;
            auto acks = reliabilitySystem->GetAcks();
            size_t ack_count = acks.size();
            for (size_t i = 0; i < ack_count; ++i )
            {
                uint32_t ack = acks[i];
                onPacketAcked(ack, reliabilityPair.first);
            }
            reliabilitySystem->Update(deltaTime);
        }
    }

    void TransportLAN::onPacketAcked(uint32_t ackedPacketSequence, int32_t nodeID)
    {
        if (m_packetCallback)
        {
            m_packetCallback(ackedPacketSequence, nodeID);
        }
    }

    void TransportLAN::setupNodeCallbacks()
    {
        node->setConnectedCallback(std::bind(&TransportLAN::onConnected, this));
        node->setDisconnectedCallback(std::bind(&TransportLAN::onDisconnected, this));
        node->setNodeConnectedCallback(std::bind(&TransportLAN::onNodeConnected, this, std::placeholders::_1));
        node->setNodeDisconnectedCallback(std::bind(&TransportLAN::onNodeDisconnected, this, std::placeholders::_1));
    }

    void TransportLAN::onConnected()
    {
        m_flowControl[0] = std::make_shared<FlowControl>();

        if (m_onConnectedCallback)
        {
            m_onConnectedCallback();
        }
    }

    void TransportLAN::onDisconnected()
    {
        m_flowControl.erase(0);

        if (m_onDisconnectedCallback)
        {
            m_onDisconnectedCallback();
        }
    }

    void TransportLAN::onNodeConnected(const NodeID nodeID)
    {
        m_flowControl[nodeID] = std::make_shared<FlowControl>();

        if (m_onNodeConnectedCallback)
        {
            m_onNodeConnectedCallback(nodeID);
        }
    }

    void TransportLAN::onNodeDisconnected(const NodeID nodeID)
    {
        m_flowControl.erase(nodeID);

        if (m_onNodeDisconnectedCallback)
        {
            m_onNodeDisconnectedCallback(nodeID);
        }
    }
}
