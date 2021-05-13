#ifndef NET_DRUDGENET_H
#define NET_DRUDGENET_H

#include "Network/DrudgeNet/include/DataTypes.h"
#include "Transport.h"
#include <map>
#include <memory>
#include <vector>
#include <functional>

namespace Net
{
    class FragmentBuffer;
    class MeasureStream;
    class Message;
    class MessageQueue;
    class ReadStream;
    class Stream;
    class WriteStream;
    class MessageFactory;
    class Channel;
    
    class DrudgeNet
    {
    public:
        typedef std::function<void(const std::shared_ptr<Message>& message,
                                   const NodeID nodeID)> MessageReceivedCallback;
        typedef std::function<void(const NodeID nodeID)> NodeConnectionCallback;
        typedef std::function<void(const NodeID nodeID, const MessageID messageID)> MessageAckedCallback;

        DrudgeNet(std::shared_ptr<MessageFactory>& messageFactory);
        ~DrudgeNet();
        
        void initialize(const TransportType transportType,
                        const Address masterServerAddress,
                        const Port masterServerConnectPort,
                        const Port meshPort,
                        const Port serverPort,
                        const Port clientPort,
                        const ProtocolID protocolID,
                        const float meshSendRate,
                        const float timeout,
                        const int32_t maxNodes);
        
        void host(const std::string& name);
        void join(const std::string& host);
        void enterLobby();
        void stop();

        void update(const float deltaTime);
        
        MessageID sendMessage(const NodeID nodeID,
                              std::shared_ptr<Message>& message,
                              bool reliable = false);
        
        void receiveMessages();
        void sendMessages();
        
        const std::shared_ptr<Transport>& getTransport() const { return m_transport; }

        void setConnectedCallback(std::function<void()> callback) { m_onConnectedCallback = callback; }
        void setDisconnectedCallback(std::function<void()> callback) { m_onDisconnectedCallback = callback; }
        void setNodeConnectedCallback(NodeConnectionCallback callback) { m_onNodeConnectedCallback = callback; }
        void setNodeDisconnectedCallback(NodeConnectionCallback callback) { m_onNodeDisconnectedCallback = callback; }
        void setMessageReceivedCallback(MessageReceivedCallback callback) { m_messageReceivedCallback = callback; }
        void setReliableMessageAckedCallback(MessageAckedCallback callback) { m_onReliableMessageAckedCallback = callback; }
        void setMasterServerConnectionCallback(std::function<void(bool)> callback) { m_masterServerConnectionCallback = callback; }
    private:
        enum PacketType
        {
            Packet_Fragment = 0,
            Packet_Single,
            PacketType_NumTypes
        };
        std::shared_ptr<MessageFactory> m_messageFactory;
        std::shared_ptr<Transport> m_transport;
        std::shared_ptr<Net::ReadStream> m_readStream;
        std::shared_ptr<Net::WriteStream> m_writeStream;
        std::shared_ptr<Net::MeasureStream> m_measureStream;
        std::shared_ptr<Net::FragmentBuffer> m_readFragmentBuffer;
        std::shared_ptr<Net::FragmentBuffer> m_writeFragmentBuffer;
        
        unsigned char* m_readBuffer;
        unsigned char* m_writeBuffer;
        unsigned char* m_packetDataBuffer;

        std::map<NodeID, std::shared_ptr<MessageQueue>> m_messagesUnreliable;
        std::map<NodeID, std::shared_ptr<MessageQueue>> m_messagesReliable;
        std::map<NodeID, std::vector<PacketSequenceID>> m_sentReliablePackets;
        
        NodeID m_fragmentSenderID;
        bool m_isConnected;

        std::function<void()> m_onConnectedCallback;
        std::function<void()> m_onDisconnectedCallback;
        NodeConnectionCallback m_onNodeConnectedCallback;
        NodeConnectionCallback m_onNodeDisconnectedCallback;
        MessageReceivedCallback m_messageReceivedCallback;
        MessageAckedCallback m_onReliableMessageAckedCallback;
        std::function<void(bool)> m_masterServerConnectionCallback;
        
        void processReadStream(const NodeID senderNode, const int receivedBytes);
        void processReadStreamFragment(const NodeID senderNode, const int receivedBytes);

        void SendMessages(std::map<NodeID, std::shared_ptr<MessageQueue>> queue, bool reliable);
        
        bool SendPacket(NodeID nodeID,
                        const unsigned char data[],
                        int32_t size);
        
        bool SendFragment(const NodeID nodeID,
                          const unsigned char data[],
                          const int32_t size,
                          PacketFragmentID fragmentID,
                          uint8_t fragmentCount);

        void onPacketAcked(PacketSequenceID ackedPacketSequence, NodeID nodeID);
        void onConnected();
        void onDisconnected();
        void onNodeConnected(const NodeID nodeID);
        void onNodeDisconnected(const NodeID nodeID);
        void onMasterServerConnection(bool connected);

        static void PadDataToNearestByte(std::shared_ptr<Net::Stream> stream);
        static const uint64_t getTimeStamp();
    };
}

#endif /* NET_DRUDGENET_H */
