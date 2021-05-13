#ifndef NET_MESH_H
#define NET_MESH_H

#include "Socket.h"
#include "Network/DrudgeNet/include/DataTypes.h"
#include <vector>
#include <map>

namespace Net
{
    class Mesh
    {
    public:
        struct NodeState
        {
            enum Mode { Disconnected, ConnectionAccept, Connected };
            Mode mode;
            float timeoutAccumulator;
            Address address;
            NodeID nodeId;
            NodeState()
            {
                mode = Disconnected;
                address = Address();
                nodeId = -1;
                timeoutAccumulator = 0.0f;
            }
        };

        Mesh(ProtocolID protocolId,
             int maxNodes = 255,
             float sendRate = 0.25f,
             float timeout = 10.0f);
        
        ~Mesh();
        
        bool Start(Port port);
        
        void Stop();
        
        void Update(float deltaTime);
        
        bool IsNodeConnected(NodeID nodeId);
        
        Address GetNodeAddress(NodeID nodeId);
        
        int GetMaxAllowedNodes() const;
        
        void Reserve(NodeID nodeId, const Address & address );
        
    protected:
        void ReceivePackets();
        
        void ProcessPacket( const Address & sender, unsigned char data[], int size );
        
        void SendPackets( float deltaTime );
        
        void CheckForTimeouts( float deltaTime );
        
    private:
        ProtocolID protocolId;
        float sendRate;
        float timeout;
        
        Socket socket;
        std::vector<NodeState> nodes;
        typedef std::map<Address, NodeState*> AddrToNode;
        AddrToNode addr2node;
        bool running;
        float sendAccumulator;
        
        void writeCRCWithProtocolID(unsigned char* data, const uint32_t size);
    };
}

#endif /* Mesh_hpp */
