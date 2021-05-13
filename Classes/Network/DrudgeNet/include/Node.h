#ifndef NET_NODE_H
#define NET_NODE_H

#include "Socket.h"
#include "Network/DrudgeNet/include/DataTypes.h"
#include <stack>
#include <vector>
#include <map>
#include <functional>

namespace Net
{
    class Node
    {
    public:
        Node(ProtocolID protocolId,
             float sendRate = 0.25f,
             float timeout = 10.0f,
             int32_t maxPacketSize = 1024);
        
        ~Node();
        
        bool Start(Port port);
        void Stop();
        
        void Join(const Address& address);
        
        bool IsJoining() const { return m_state == Joining; }
        bool JoinFailed() const { return m_state == JoinFail; }
        bool IsConnected() const { return m_state == Joined; }
        
        NodeID GetLocalNodeId() const { return m_localNodeID; }
        
        void Update(float deltaTime);
        bool IsNodeConnected(NodeID nodeID);
        
        const Address& GetNodeAddress(NodeID nodeID) const;
        int32_t GetMaxNodes() const;
        
        bool SendPacket(NodeID nodeID, const unsigned char data[], int size);
        int ReceivePacket(NodeID& nodeID, unsigned char data[], int size);
        
        void setConnectedCallback(std::function<void()> callback) { m_onConnectedCallback = callback; }
        void setDisconnectedCallback(std::function<void()> callback) { m_onDisconnectedCallback = callback; }
        void setNodeConnectedCallback(std::function<void(const NodeID nodeID)> callback) { m_onNodeConnectedCallback = callback; }
        void setNodeDisconnectedCallback(std::function<void(const NodeID nodeID)> callback) { m_onNodeDisconnectedCallback = callback; }

    protected:
        void ReceivePackets();
        void ProcessPacket(const Address& sender, unsigned char data[], int size);
        void SendPackets(float deltaTime);
        void CheckForTimeout(float deltaTime);
        void ClearData();
        
    private:
        enum State
        {
            Disconnected,
            Joining,
            Joined,
            JoinFail
        };
        
        struct NodeState
        {
            bool connected;
            Address address;
            NodeID nodeID;
            NodeState()
            {
                connected = false;
                address = Address();
                nodeID = -1;
            }
        };
        
        struct BufferedPacket
        {
            NodeID nodeID;
            std::vector<uint8_t> data;
        };
        
        typedef std::stack<BufferedPacket*> PacketBuffer;
        PacketBuffer m_receivedPackets;
        typedef std::map<Address, NodeState*> AddrToNode;
        AddrToNode m_addr2node;
        Socket m_socket;
        std::vector<NodeState> m_nodes;
        Address m_meshAddress;

        ProtocolID m_protocolId;
        float m_sendRate;
        float m_timeout;
        int32_t m_maxPacketSize;
        State m_state;
        float m_meshSendAccumulator;
        float m_timeoutAccumulator;
        int32_t m_localNodeID;
        bool m_running;

        std::function<void()> m_onConnectedCallback;
        std::function<void()> m_onDisconnectedCallback;
        std::function<void(const NodeID nodeID)> m_onNodeConnectedCallback;
        std::function<void(const NodeID nodeID)> m_onNodeDisconnectedCallback;

        void ProcessMeshPacket(unsigned char data[], const int32_t size);
        void OnMeshConnectionAccepted(const NodeID nodeID, const int32_t numNodes);
        void onMeshUpdate(unsigned char data[], const int32_t size);

        void ProcessNodePacket(const Address& sender, unsigned char data[], const int32_t size);
        void writeCRCWithProtocolID(unsigned char* data, const uint32_t size);
    };
}

#endif /* Node_hpp */
