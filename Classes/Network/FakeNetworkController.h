#ifndef FakeNetworkController_h
#define FakeNetworkController_h

#include "INetworkController.h"
#include "Network/NetworkMessages.h"

namespace Net
{
    class Message;
    class ReadStream;
    class WriteStream;
    class MessageFactory;
}

class FakeNet;

class FakeNetworkController : public INetworkController
{
public:
    FakeNetworkController(std::shared_ptr<FakeNet> fakeNet);
    ~FakeNetworkController();
    
    void initialize(const NetworkMode mode) override;
    void terminate() override;
    
    void host(const std::string& name) override {}
    void join(const std::string& host) override {}
    void enterLobby() override {}
    void stop() override {}
    
    Net::MessageID sendMessage(const Net::NodeID nodeID,
                               std::shared_ptr<Net::Message>& message,
                               bool reliable = false) override;
    
    void update(const float deltaTime) override;

    void receiveMessages() override {};
    void sendMessages() override {};

    const std::shared_ptr<Net::Transport> getTransport() override { return nullptr; }
    float getSentBandwidth(const Net::NodeID nodeID) override;
    float getAckedBandwidth(const Net::NodeID nodeID) override;
    float getRoundTripTime(const Net::NodeID nodeID) override;
    const Net::NodeID getLocalNodeID() const override { return 0; };

    bool isBroadcasting() const override { return true; }
    bool isListening() const override { return true; }
    bool isConnected() const override { return true; }
    
private:
    static const int BUFFER_SIZE;

    std::shared_ptr<FakeNet> m_fakeNet;

    unsigned char* m_readBuffer;
    unsigned char* m_writeBuffer;
    std::shared_ptr<Net::ReadStream> m_readStream;
    std::shared_ptr<Net::WriteStream> m_writeStream;
    std::shared_ptr<Net::MessageFactory> m_messageFactory;
    float m_sentBytes;
    float m_ackedBytes;
    float m_sentBandwidth;
    float m_ackedBandwidth;

    std::function<void(const uint8_t playerID, const std::shared_ptr<Net::Message> data)> m_clientCallback;
    std::function<void(const std::shared_ptr<Net::Message> data)> m_serverCallback;

    void onClientDataReceived(const uint8_t playerID, const unsigned char* data, const size_t dataSize);
    void onServerDataReceived(const unsigned char* data, const size_t dataSize);
};

#endif /* FakeNetworkController_h */
