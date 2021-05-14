#ifndef MasterServer_h
#define MasterServer_h

#include "Address.h"
#include "DataTypes.h"
#include "MasterServerMessages.h"
#include <memory>
#include <functional>
#include <vector>
#include <stdint.h>

namespace Net {
class Message;
class Socket;
class ReadStream;
class WriteStream;

class MasterServer
{
public:
    MasterServer(const Net::Port serverPort = 12345,
                 const Net::ProtocolID protocolID = 666666,
                 const float timeOut = 10.0f);
    ~MasterServer();
    bool start();
    void stop();
    
    void update(const float deltaTime);
    
    void setInfoCallback(std::function<void(const std::string&)> cb) { m_infoCallback = cb; }
    void setHostConnectedCallback(std::function<void(const Net::Address& address)> cb) { m_onConnectedCallback = cb; }
    void setHostDisconnectedCallback(std::function<void(const Net::Address& address)> cb) { m_onDisconnectedCallback = cb; }
private:
    std::unique_ptr<Net::Socket> m_socket;
    std::unique_ptr<Net::ReadStream> m_readStream;
    std::unique_ptr<Net::WriteStream> m_writeStream;
    unsigned char* m_readBuffer;
    unsigned char* m_writeBuffer;
    std::function<void(const std::string&)> m_infoCallback;
    std::function<void(const Net::Address& address)> m_onConnectedCallback;
    std::function<void(const Net::Address& address)> m_onDisconnectedCallback;

    Net::Port m_port;
    Net::ProtocolID m_protocolID;
    float m_timeout;
    bool m_running;
    
    struct GameClientData {
        Net::Address address;
        Net::Port clientPort;
        float timeSinceUpdate;
    };
    
    std::vector<GameHostMasterData> m_connectedHosts;
    std::vector<GameClientData> m_connectedClients;

    void readSocketData();
    void processReceivedData(const Net::Address& sender,
                             const ssize_t receivedBytes);
    void updateConnections(const float deltaTime);
    bool validateDataHeader(const ssize_t receivedBytes);
    void writeHeaderData(const int dataBytes);
    void sendMessage(const std::shared_ptr<Net::Message>& message, const Net::Address& address);
    void sendPing(const Net::Address& address, const bool isPong);
    void onGameHostUpdate(const Net::Address& sender);
    void onGameListRequest(const Net::Address& sender);
    void onGameConnectionRequest(const Net::Address& sender);
    void onInfo(const std::string& info);
};

}

#endif /* MasterServer_h */
