#ifndef NETWORK_CONTROLLER_H
#define NETWORK_CONTROLLER_H

#include "INetworkController.h"
#include "NetworkMessages.h"
#include "Network/DrudgeNet/include/DrudgeNet.h"

class NetworkModel;
class NetworkMessageFactory;
class GameSettings;

namespace Net {
    class Stream;
    class TransportLAN;
}

class NetworkController : public INetworkController
{
public:
    NetworkController(std::shared_ptr<NetworkModel> model,
                      std::shared_ptr<GameSettings> gameSettings);
    virtual ~NetworkController();
    
    void initialize(const NetworkMode mode) override;
    void terminate() override;
    
    void host(const std::string& name) override;
    void join(const std::string& host) override;
    void enterLobby() override;
    void stop() override;
    
    Net::MessageID sendMessage(const Net::NodeID nodeID,
                               std::shared_ptr<Net::Message>& message,
                               bool reliable = false) override;
    
    void update(const float deltaTime) override;
    void receiveMessages() override;
    void sendMessages() override;

    const std::shared_ptr<Net::Transport> getTransport() override { return m_drudgeNet->getTransport(); }
    float getRoundTripTime(const Net::NodeID nodeID) override;
    const Net::NodeID getLocalNodeID() const override;
    
    bool isBroadcasting() const override { return m_isBroadcasting; }
    
    bool isListening() const override { return m_isListening; }
    bool isConnected() const override { return m_isConnected; }

    void setDeltaDataCallback(std::function<const SnapshotData&(const uint32_t)> dataCallback) override;
private:
    static const int BUFFER_SIZE;
    static const std::string SETTING_NETWORK_TYPE;
    static const std::string SETTING_NETWORK_PROTOCOL_ID;
    static const std::string SETTING_NETWORK_MASTER_SERVER_ADDRESS;
    static const std::string SETTING_NETWORK_MASTER_SERVER_PORT;
    static const std::string SETTING_NETWORK_MESH_PORT;
    static const std::string SETTING_NETWORK_SERVER_PORT;
    static const std::string SETTING_NETWORK_CLIENT_PORT;
    static const std::string SETTING_NETWORK_MESH_SEND_RATE;
    static const std::string SETTING_NETWORK_TIMEOUT;
    static const std::string SETTING_NETWORK_MAX_NODES;

    std::shared_ptr<NetworkModel> m_model;
    std::shared_ptr<GameSettings> m_gameSettings;
    std::shared_ptr<Net::DrudgeNet> m_drudgeNet;
    std::shared_ptr<NetworkMessageFactory> m_messageFactory;
    std::shared_ptr<Net::Stream> m_readStream;
    std::shared_ptr<Net::Stream> m_writeStream;
    unsigned char* m_readBuffer;
    unsigned char* m_writeBuffer;
    
    NetworkMode m_mode;

    // server data
    bool m_isBroadcasting;

    // client data
    std::function<void()> m_onConnectedCallback;
    std::function<void()> m_onDisconnectedCallback;
    
    bool m_isListening;
    bool m_isConnected;

    void processReadStream(int bytesRead, const int nodeID);
    
    void onNodeConnected(const Net::NodeID nodeID);
    void onNodeDisconnected(const Net::NodeID nodeID);
    
    void onMessageReceived(const std::shared_ptr<Net::Message>& message,
                           const Net::NodeID nodeID);
    void onMessageAcked(const Net::NodeID nodeID,
                        const Net::MessageID messageID);
    
    void onMasterSystemConnection(bool connected);

    Net::Address getAddressFromString(const std::string& string) const;
};

#endif /* NETWORK_CONTROLLER_H */
