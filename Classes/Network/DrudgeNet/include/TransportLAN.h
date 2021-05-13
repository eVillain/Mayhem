#ifndef NET_TRANSPORT_LAN_H
#define NET_TRANSPORT_LAN_H

#include "Transport.h"
#include "Network/DrudgeNet/include/DataTypes.h"
#include <vector>
#include <map>

namespace Net
{
    class ReadStream;
    class Stream;
    class WriteStream;
    class FlowControl;
    class ReliabilitySystem;

    // lan transport implementation
    //  + servers are advertised via net beacon
    //  + lan lobby is filled via net listener
    //  + a mesh runs on the server IP and manages node connections
    //  + a node runs on each transport, including a local node on the server
    class TransportLAN : public Transport
    {
    public:        
        // static interface
        
        static bool Initialize();
        
        static void Shutdown();
        
        // lan specific interface
        
        TransportLAN();
        ~TransportLAN();
        
        struct Config
        {
            Port meshPort;
            Port serverPort;
            Port clientPort;
            Port beaconPort;
            Port listenerPort;
            ProtocolID protocolId;
            float meshSendRate;
            float timeout;
            int32_t maxNodes;
            
            Config()
            {
                meshPort = 30000;
                clientPort = 30001;
                serverPort = 30002;
                beaconPort = 40000;
                listenerPort = 40001;
                protocolId = 666666;
                meshSendRate = 1.0f / 5.0f;
                timeout = 10.0f;
                maxNodes = 100;
            }
        };
        
        void Configure(Config & config);
        
        const Config & GetConfig() const;
        
        bool StartServer(const std::string& name) override;
        
        bool ConnectClient(const std::string& server) override;
        
        bool ConnectFailed() const;
        
        bool EnterLobby() override;
        
        int32_t GetLobbyEntryCount() const override;
 
        bool GetLobbyEntryAtIndex(int32_t index,
                                  LobbyEntry& entry) override;
                
        // implement transport interface
        
        bool IsConnected() const override;
        bool IsNodeConnected(NodeID nodeId) override;
        NodeID GetLocalNodeId() const override;
        const Address& GetNodeAddress(NodeID nodeId) const override;
        int32_t GetMaxNodes() const override;

        bool SendPacket(NodeID nodeID,
                        const unsigned char data[],
                        int32_t size) override;
        
        int32_t ReceivePacket(NodeID & nodeID,
                              unsigned char data[],
                              int32_t size) override;
        
        std::shared_ptr<ReliabilitySystem> GetReliability(NodeID nodeId) override;
        const std::map<NodeID, std::shared_ptr<FlowControl>>& getFlowControl() const { return m_flowControl; }

        void Update(float deltaTime) override;
        void Stop() override;

        TransportType GetType() const override;
        
        void setPacketAckedCallback(PacketAckedCallback callback) override { m_packetCallback = callback; }
        void setConnectedCallback(std::function<void()> callback) override { m_onConnectedCallback = callback; }
        void setDisconnectedCallback(std::function<void()> callback) override { m_onDisconnectedCallback = callback; }
        void setNodeConnectedCallback(std::function<void(const NodeID nodeID)> callback) override { m_onNodeConnectedCallback = callback; }
        void setNodeDisconnectedCallback(std::function<void(const NodeID nodeID)> callback) override { m_onNodeDisconnectedCallback = callback; }
        
    private:
        Config m_config;
        class Mesh* mesh;
        class Node* node;
        class Beacon* beacon;
        class Listener* listener;
        std::shared_ptr<Net::ReadStream> m_readStream;
        std::shared_ptr<Net::WriteStream> m_writeStream;

        unsigned char* m_readBuffer;
        unsigned char* m_writeBuffer;

        int32_t m_tickAccumulator;
        float m_connectAccumulator;

        bool m_connectingByName;
        bool m_connectFailed;
        
        char connectName[65];
        
        PacketAckedCallback m_packetCallback;
        std::function<void()> m_onConnectedCallback;
        std::function<void()> m_onDisconnectedCallback;
        std::function<void(const NodeID nodeID)> m_onNodeConnectedCallback;
        std::function<void(const NodeID nodeID)> m_onNodeDisconnectedCallback;

        std::map<NodeID, std::shared_ptr<ReliabilitySystem>> m_reliabilitySystems;
        std::map<NodeID, std::shared_ptr<FlowControl>> m_flowControl;
        std::map<NodeID, float> m_sendAccumulators;

        void AttemptConnection(const float deltaTime);
        void UpdateReliabilitySystems(const float deltaTime);
        
        void WriteHeader(unsigned char* header,
                         PacketSequenceID sequence,
                         uint32_t ack,
                         uint32_t ack_bits);
        
        void ReadHeader(const unsigned char* header,
                        PacketSequenceID & sequence,
                        uint32_t & ack,
                        uint32_t & ack_bits);
        
        void onPacketAcked(PacketSequenceID ackedPacketSequence,
                           NodeID nodeID);
        
        void setupNodeCallbacks();
        void onConnected();
        void onDisconnected();
        void onNodeConnected(const NodeID nodeID);
        void onNodeDisconnected(const NodeID nodeID);
    };
}

#endif /* TransportLAN_h */
