#ifndef INETWORK_CONTROLLER_H
#define INETWORK_CONTROLLER_H

#include "NetworkConstants.h"
#include "Network/DrudgeNet/include/DataTypes.h"
#include "NetworkMessages.h"
#include <functional>
#include <map>
#include <memory>

namespace Net {
    class Message;
    class Stream;
    class Transport;
}

class INetworkController
{
public:
    typedef std::function<void(const Net::NodeID nodeID)> NodeConnectionCallback;
    typedef std::function<void(const std::shared_ptr<Net::Message>& message,
                               const Net::NodeID nodeID)> MessageReceivedCallback;
    typedef std::function<void(const Net::NodeID nodeID,
                               const Net::MessageID messageID)> MessageAckedCallback;
    typedef std::function<void(const std::string& message)> NetControllerMessageCallback;
    virtual ~INetworkController() {}
    
    virtual void initialize(const NetworkMode mode) = 0;
    virtual void terminate() = 0;
    
    virtual void host(const std::string& name) = 0;
    virtual void join(const std::string& host) = 0;
    virtual void enterLobby() = 0;
    virtual void stop() = 0;
    
    virtual Net::MessageID sendMessage(const Net::NodeID nodeID,
                                       std::shared_ptr<Net::Message>& message,
                                       bool reliable = false) = 0;

    virtual void update(const float deltaTime) = 0;

    virtual void setDeltaDataCallback(std::function<const SnapshotData&(const uint32_t)> dataCallback) {};

    virtual void receiveMessages() = 0;
    virtual void sendMessages() = 0;
    
    virtual const std::shared_ptr<Net::Transport> getTransport() = 0;
    virtual float getRoundTripTime(const Net::NodeID nodeID) = 0;
    virtual const Net::NodeID getLocalNodeID() const = 0;

    void setMessageReceivedCallback(MessageReceivedCallback callback) { m_messageReceivedCallback = callback; }
    void setNodeConnectedCallback(NodeConnectionCallback callback) { m_onNodeConnectedCallback = callback; }
    void setNodeDisconnectedCallback(NodeConnectionCallback callback) { m_onNodeDisconnectedCallback = callback; }
    void setReliableMessageAckedCallback(MessageAckedCallback callback) { m_onReliableMessageAckedCallback = callback; }
    void setControllerMessageCallback(NetControllerMessageCallback callback) { m_controllerMessageCallback = callback; }

    //-----//
    void addMessageCallback(const Net::MessageID messageID, MessageReceivedCallback callback)
    {
        m_dispatchMap[messageID] = callback;
    }
    void removeMessageCallback(const Net::MessageID messageID)
    {
        auto it = m_dispatchMap.find(messageID);
        if (it != m_dispatchMap.end())
        {
            m_dispatchMap.erase(it);
        }
    }

    virtual bool isBroadcasting() const = 0;
    virtual bool isListening() const = 0;
    virtual bool isConnected() const = 0;

protected:
    NetworkMode m_mode;

    MessageReceivedCallback m_messageReceivedCallback;
    NodeConnectionCallback m_onNodeConnectedCallback;
    NodeConnectionCallback m_onNodeDisconnectedCallback;
    MessageAckedCallback m_onReliableMessageAckedCallback;
    NetControllerMessageCallback m_controllerMessageCallback;

    std::map<uint8_t, MessageReceivedCallback> m_dispatchMap;
    
    void controllerMessage(const std::string& message)
    {
        if (m_controllerMessageCallback) m_controllerMessageCallback(message);
    }
};

#endif /* INETWORK_CONTROLLER_H */
