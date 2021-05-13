#ifndef NET_TRANSPORT_H
#define NET_TRANSPORT_H

#include "DataTypes.h"
#include "Address.h"
#include <functional>
#include <memory>

namespace Net
{
    class ReliabilitySystem;

    enum TransportType
    {
        Transport_None,
        Transport_LAN,
		Transport_IP,
    };
    
    // abstract network transport interface
    //  + implement this interface for different transport layers
    //  + use the reliability system classes to implement seq/ack based reliability
    
    class Transport
    {
    public:
        typedef std::function<void(PacketSequenceID ackedPacketSequence,
                                   NodeID nodeID)> PacketAckedCallback;

        struct LobbyEntry {
            Net::Address address;
            std::string name;
            uint32_t maxClients;
            uint32_t currentClients;
        };

        // transport interface
        
        virtual ~Transport() {};
        
        virtual bool StartServer(const std::string& name) = 0;
        
        virtual bool ConnectClient(const std::string& server) = 0;
        
        virtual bool IsConnected() const = 0;

        virtual bool IsNodeConnected(NodeID nodeId) = 0;
        
        virtual int GetLocalNodeId() const = 0;
        
        virtual const Address& GetNodeAddress(NodeID nodeId) const = 0;
        
        virtual int GetMaxNodes() const = 0;
        
        virtual bool SendPacket(NodeID nodeId,
                                const unsigned char data[],
                                int size) = 0;
        
        virtual int ReceivePacket(NodeID & nodeId,
                                  unsigned char data[],
                                  int size) = 0;
        
        virtual std::shared_ptr<ReliabilitySystem> GetReliability(NodeID nodeId) = 0;
        
        virtual void Update(float deltaTime) = 0;

        virtual void Stop() = 0;

        virtual TransportType GetType() const = 0;
        
        virtual bool EnterLobby() = 0;
               
        virtual int32_t GetLobbyEntryCount() const = 0;
        
        virtual bool GetLobbyEntryAtIndex(int32_t index,
                                          LobbyEntry& entry) = 0;
        
        virtual void setPacketAckedCallback(PacketAckedCallback callback) = 0;
        virtual void setConnectedCallback(std::function<void()> callback) = 0;
        virtual void setDisconnectedCallback(std::function<void()> callback) = 0;
        virtual void setNodeConnectedCallback(std::function<void(const NodeID nodeID)> callback) = 0;
        virtual void setNodeDisconnectedCallback(std::function<void(const NodeID nodeID)> callback) = 0;
    };
}

#endif /* NET_TRANSPORT_H */
