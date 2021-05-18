#include "TransportIP.h"

#include "Address.h"
#include "Socket.h"
//#include "Beacon.h"
//#include "Listener.h"
#include "MasterServerConnection.h"
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
    TransportIP::TransportIP()
    : m_mesh(nullptr)
    , m_node(nullptr)
    , m_masterServerConnection(nullptr)
    , m_readStream(nullptr)
    , m_writeStream(nullptr)
    , m_readBuffer(nullptr)
    , m_writeBuffer(nullptr)
    , m_tickAccumulator(-1)
    , m_connectAccumulator(0.0f)
    , m_connectFailed(false)
    , m_packetCallback(nullptr)
    , m_onConnectedCallback(nullptr)
    , m_onDisconnectedCallback(nullptr)
    , m_onNodeConnectedCallback(nullptr)
    , m_onNodeDisconnectedCallback(nullptr)
    , m_masterServerConnectionCallback(nullptr)
    {
        m_readBuffer = new unsigned char[BUFFER_SIZE_BYTES];
        m_writeBuffer = new unsigned char[BUFFER_SIZE_BYTES];
        
        m_readStream = std::make_shared<Net::ReadStream>(m_readBuffer, BUFFER_SIZE_BYTES);
        m_writeStream = std::make_shared<Net::WriteStream>(m_writeBuffer, BUFFER_SIZE_BYTES);
    }
    
    TransportIP::~TransportIP()
    {
        Stop();
    }
    
    void TransportIP::Configure(Config& config)
    {
        assert(!m_node);
        assert(!m_mesh);
        assert(!m_masterServerConnection);
        m_config = config;
    }
    
    const TransportIP::Config& TransportIP::GetConfig() const
    {
        return m_config;
    }
    
    bool TransportIP::StartServer(const std::string& name)
    {
        assert(!m_node);
        assert(!m_mesh);
        assert(!m_masterServerConnection);
        
        m_mesh = std::make_shared<Mesh>(m_config.protocolID, m_config.maxNodes, m_config.meshSendRate, m_config.timeout);
        if (!m_mesh->Start(m_config.meshPort))
        {
            printf("TransportIP:failed to start m_mesh on port %d\n", m_config.meshPort);
            Stop();
            return false;
        }
        
        m_node = std::make_shared<Node>(m_config.protocolID, m_config.meshSendRate, m_config.timeout, MAXIMUM_TRANSMISSION_UNIT_BYTES);
        if (!m_node->Start(m_config.serverPort))
        {
            printf("TransportIP:failed to start node on port %d\n", m_config.serverPort);
            Stop();
            return false;
        }
        setupNodeCallbacks();
        
        m_mesh->Reserve(0, Address(127, 0, 0, 1, m_config.serverPort));
        m_node->Join(Address(127, 0, 0, 1, m_config.meshPort));

        m_masterServerConnection = std::make_shared<MasterServerConnection>(MasterServerConnection::Mode::MODE_GAME_HOST,
                                                                            m_config.masterServerConnectPort,
                                                                            m_config.protocolID,
                                                                            m_config.timeout);
        
        m_masterServerConnection->setConnectCallback(std::bind(&TransportIP::onMasterServerConnected, this));
        m_masterServerConnection->setDisconnectCallback(std::bind(&TransportIP::onMasterServerDisconnected, this));
        m_masterServerConnection->connect(m_config.masterServerAddress);
        
        printf("TransportIP: started mesh on port %d\n", m_config.meshPort);

        return true;
    }
    
    bool TransportIP::ConnectClient(const std::string& server)
    {
        assert(!m_node);
        assert(!m_mesh);
        assert(m_masterServerConnection);

        auto it = std::find_if(m_lobbyEntries.begin(), m_lobbyEntries.end(), [server](const LobbyEntry& entry){
            return server == entry.name;
        });
        if (it == m_lobbyEntries.end())
        {
            return false;
        }
        const auto& host = *it;
        m_masterServerConnection->requestLinkToHost(host.address, m_config.clientPort);
        
        printf("IP Transport: client connect to address: %s\n", host.address.GetString().c_str());
        m_node = std::make_shared<Node>(m_config.protocolID, m_config.meshSendRate, m_config.timeout, MAXIMUM_TRANSMISSION_UNIT_BYTES);
        if (!m_node->Start(m_config.clientPort))
        {
            printf("IP Transport: failed to start client m_node on port %d\n", m_config.clientPort);
            Stop();
            return 1;
        }
        setupNodeCallbacks();
        m_node->Join(host.address);

        return true;
    }
    
    bool TransportIP::IsConnected() const
    {
        return m_node && m_node->IsConnected();
    }
    
    bool TransportIP::ConnectFailed() const
    {
        return m_node && m_node->JoinFailed();
    }
    
    bool TransportIP::EnterLobby()
    {
        assert(!m_masterServerConnection);
        printf("TransportIP: enter lobby\n");
        m_masterServerConnection = std::make_shared<MasterServerConnection>(MasterServerConnection::Mode::MODE_GAME_CLIENT,
                                                                            m_config.masterServerConnectPort,
                                                                            m_config.protocolID,
                                                                            m_config.timeout);
        m_masterServerConnection->setConnectCallback(std::bind(&TransportIP::onMasterServerConnected, this));
        m_masterServerConnection->setDisconnectCallback(std::bind(&TransportIP::onMasterServerDisconnected, this));
        m_masterServerConnection->setHostListCallback([this](const std::vector<LobbyEntry>& hosts, const uint32_t, const uint32_t){
            std::cout << "TransportIP client host list received\n";
            if (hosts.empty())
            {
                return;
            }
            // Save hosts locally
            m_lobbyEntries.clear();
            for (const auto& hostData : hosts)
            {
                LobbyEntry entry = {hostData.address, hostData.name, hostData.maxClients, hostData.currentClients};
                m_lobbyEntries.push_back(entry);
            }
        });
        m_masterServerConnection->connect(m_config.masterServerAddress);
        return true;
    }
    
    int32_t TransportIP::GetLobbyEntryCount() const
    {
        return (int32_t)m_lobbyEntries.size();
    }
    
    bool TransportIP::GetLobbyEntryAtIndex(int32_t index, LobbyEntry& entry)
    {
        if (index >= m_lobbyEntries.size())
        {
            return false;
        }
        const auto lobbyEntry = m_lobbyEntries.at(index);
        entry.address = lobbyEntry.address;
        entry.name = lobbyEntry.name;
        entry.maxClients = lobbyEntry.maxClients;
        entry.currentClients = lobbyEntry.currentClients;
        return true;
    }
    
    void TransportIP::Stop()
    {
        printf( "TransportIP: stop\n" );
        m_mesh = nullptr;
        m_node = nullptr;
        m_masterServerConnection = nullptr;
        m_connectFailed = false;
        
        memset(m_readBuffer, 0, BUFFER_SIZE_BYTES);
        memset(m_writeBuffer, 0, BUFFER_SIZE_BYTES);
        m_readStream->Clear();
        m_writeStream->Clear();
    }
    
    // implement transport interface
    
    bool TransportIP::IsNodeConnected( NodeID m_nodeId )
    {
        assert(m_node);
        return m_node->IsNodeConnected( m_nodeId );
    }
    
    NodeID TransportIP::GetLocalNodeId() const
    {
        assert(m_node);
        return m_node->GetLocalNodeId();
    }

    const Address& TransportIP::GetNodeAddress(NodeID m_nodeId) const
    {
        assert(m_node);
        return m_node->GetNodeAddress(m_nodeId);
    }
    
    int32_t TransportIP::GetMaxNodes() const
    {
        assert(m_node);
        return m_node->GetMaxNodes();
    }
  
    bool TransportIP::SendPacket(NodeID m_nodeID,
                                 const unsigned char data[],
                                 int32_t size)
    {
        assert(m_node);
        assert(size < BUFFER_SIZE_BYTES);
        assert(m_writeStream->GetBitsProcessed() == 0);
        
        const float sendRate = m_flowControl[m_nodeID]->GetSendRate();
        if (m_sendAccumulators[m_nodeID] < 1.0f / sendRate)
        {
            return false;
        }
        
        m_sendAccumulators[m_nodeID] -= 1.0f / sendRate;
        
        std::shared_ptr<ReliabilitySystem> reliabilitySystem = GetReliability(m_nodeID);
        uint32_t seq = reliabilitySystem->GetLocalSequence();
        uint32_t ack = reliabilitySystem->GetRemoteSequence();
        uint32_t ack_bits = reliabilitySystem->GenerateAckBits();
        
        m_writeStream->SerializeInteger(seq);
        m_writeStream->SerializeInteger(ack);
        m_writeStream->SerializeInteger(ack_bits);
        
        const int32_t headerBytes = m_writeStream->GetBitsProcessed() / 8;
        memcpy(m_writeBuffer + headerBytes, data, size);
        
        bool success = m_node->SendPacket(m_nodeID, m_writeBuffer, size + headerBytes);
        if (success)
        {
            reliabilitySystem->PacketSent(size);
        }
        else
        {
            printf("TransportIP send failed, m_node said no\n");
        }
        
        memset(m_writeBuffer, 0, size + headerBytes);
        m_writeStream->Clear();
        
        return success;        
    }
    
    int32_t TransportIP::ReceivePacket(NodeID& m_nodeId,
                                        unsigned char data[],
                                        int32_t size)
    {
        assert(m_node);
        
        int32_t receivedBytes = m_node->ReceivePacket(m_nodeId, m_readBuffer, BUFFER_SIZE_BYTES);
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
        
        std::shared_ptr<ReliabilitySystem> reliabilitySystem = GetReliability(m_nodeId);
        reliabilitySystem->PacketReceived(packet_sequence, receivedBytes - headerBytes);
        reliabilitySystem->ProcessAck(packet_ack, packet_ack_bits);

        memcpy(data, m_readBuffer + headerBytes, receivedBytes - headerBytes);
    
        memset(m_readBuffer, 0, receivedBytes);
        m_readStream->Clear();

//        printf("TransportIP Received %i bytes from m_node  %i, sequence %i\n", receivedBytes, m_nodeId, packet_sequence);

        return receivedBytes - headerBytes;
    }
    
    std::shared_ptr<ReliabilitySystem> TransportIP::GetReliability(NodeID m_nodeId)
    {
        if (m_reliabilitySystems.find(m_nodeId) == m_reliabilitySystems.end())
        {
            m_reliabilitySystems[m_nodeId] = std::make_shared<ReliabilitySystem>();
        }
        return m_reliabilitySystems[m_nodeId];
    }
    
    void TransportIP::Update(float deltaTime)
    {
        m_tickAccumulator++;
        
        if (m_masterServerConnection)
        {
            m_masterServerConnection->update(deltaTime);
        }
        
        if (m_mesh)
        {
            m_mesh->Update(deltaTime);
            const int maxNodes = GetMaxNodes();
            for (int m_nodeID = 1; m_nodeID < maxNodes; m_nodeID++)
            {
                if (IsNodeConnected(m_nodeID))
                {
                    m_flowControl[m_nodeID]->Update(deltaTime, GetReliability(m_nodeID)->GetRoundTripTime() * 1000.f);
                }
            }
        }
        if (m_node)
        {
            m_node->Update(deltaTime);
            
            if (m_node->IsConnected())
            {
                m_flowControl[0]->Update(deltaTime, GetReliability(0)->GetRoundTripTime() * 1000.f);
            }
        }
        if (m_mesh || m_node)
        {
            UpdateReliabilitySystems(deltaTime);
            
            for (auto& accumulator : m_sendAccumulators)
            {
                accumulator.second = accumulator.second + deltaTime;
            }
        }
    }
    
    TransportType TransportIP::GetType() const
    {
        return Transport_LAN;
    }
        
    void TransportIP::UpdateReliabilitySystems(const float deltaTime)
    {
        for (auto& reliabilityPair : m_reliabilitySystems)
        {
            std::shared_ptr<ReliabilitySystem>& reliabilitySystem = reliabilityPair.second;
            auto acks = reliabilitySystem->GetAcks();
            size_t ack_count = acks.size();
            for ( int32_t i = 0; i < ack_count; ++i )
            {
                uint32_t ack = acks[i];
                onPacketAcked(ack, reliabilityPair.first);
            }
            reliabilitySystem->Update(deltaTime);
        }
    }

    void TransportIP::onPacketAcked(uint32_t ackedPacketSequence, int32_t m_nodeID)
    {
        if (m_packetCallback)
        {
            m_packetCallback(ackedPacketSequence, m_nodeID);
        }
    }

    void TransportIP::setupNodeCallbacks()
    {
        m_node->setConnectedCallback(std::bind(&TransportIP::onConnected, this));
        m_node->setDisconnectedCallback(std::bind(&TransportIP::onDisconnected, this));
        m_node->setNodeConnectedCallback(std::bind(&TransportIP::onNodeConnected, this, std::placeholders::_1));
        m_node->setNodeDisconnectedCallback(std::bind(&TransportIP::onNodeDisconnected, this, std::placeholders::_1));
    }

    void TransportIP::onConnected()
    {
        m_flowControl[0] = std::make_shared<FlowControl>();

        if (m_onConnectedCallback)
        {
            m_onConnectedCallback();
        }
    }

    void TransportIP::onDisconnected()
    {
        m_flowControl.erase(0);

        if (m_onDisconnectedCallback)
        {
            m_onDisconnectedCallback();
        }
    }

    void TransportIP::onNodeConnected(const NodeID m_nodeID)
    {
        m_flowControl[m_nodeID] = std::make_shared<FlowControl>();

        if (m_onNodeConnectedCallback)
        {
            m_onNodeConnectedCallback(m_nodeID);
        }
    }

    void TransportIP::onNodeDisconnected(const NodeID m_nodeID)
    {
        m_flowControl.erase(m_nodeID);

        if (m_onNodeDisconnectedCallback)
        {
            m_onNodeDisconnectedCallback(m_nodeID);
        }
    }

    void TransportIP::onMasterServerConnected()
    {
        std::cout << "TransportIP connected to master server\n";
        if (m_mesh)
        {
            m_masterServerConnection->sendHostUpdate(100, 0, GameHostState::InLobby, m_config.serverPort, true);
        }
        else
        {
            m_masterServerConnection->requestGameList(0, 100);
        }

        if (m_masterServerConnectionCallback)
        {
            m_masterServerConnectionCallback(true);
        }
    }

    void TransportIP::onMasterServerDisconnected()
    {
        std::cout << "TransportIP disconnected from master server\n";

        if (m_masterServerConnectionCallback)
        {
            m_masterServerConnectionCallback(false);
        }
        
        m_masterServerConnection->connect(m_config.masterServerAddress);
    }
}
