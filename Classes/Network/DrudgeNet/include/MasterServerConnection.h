#ifndef MasterServerConnection_h
#define MasterServerConnection_h

#include "MasterServerMessages.h"
#include "Address.h"
#include <functional>

namespace Net {
class Message;
class Socket;
class ReadStream;
class WriteStream;

class MasterServerConnection
{
public:
    enum Mode {
        MODE_GAME_HOST,
        MODE_GAME_CLIENT
    };
    
    enum State {
        DISCONNECTED,
        CONNECTING,
        CONNECT_FAILED,
        CONNECTED
    };
    
    MasterServerConnection(const Mode mode,
                           const Net::Port port = 30001,
                           const Net::ProtocolID protocolID = 666666,
                           const float timeOut = 10.0f);
    ~MasterServerConnection();

    bool connect(const Net::Address& address);
    void disconnect();

    bool isConnected() const { return m_state == State::CONNECTED; }
    
    void update(const float deltaTime);
    
    void requestGameList(const uint32_t startRange,
                         const uint32_t endRange);
    void requestLinkToHost(const Net::Address& hostAddress,
                           const Net::Port clientPort);
    void sendHostUpdate(const uint32_t totalCapacity,
                        const uint32_t currentPlayerCount,
                        const GameHostState state,
                        const Net::Port gameClientPort,
                        const bool allowsJoining);
    
    void setConnectCallback(std::function<void()> cb) { m_connectCallback = cb; }
    void setDisconnectCallback(std::function<void()> cb) { m_disconnectCallback = cb; }
    void setInfoCallback(std::function<void(const std::string&)> cb) { m_infoCallback = cb; }
    void setHostListCallback(std::function<void(const std::vector<Net::Transport::LobbyEntry>& hosts,
                                                const uint32_t startRange,
                                                const uint32_t endRange)> cb) { m_hostListCallback = cb; }
    void setClientConnectCallback(std::function<void(const Net::Address& address,
                                                     const bool success)> cb) { m_clientConnectCallback = cb; }
    void setHostConnectCallback(std::function<void(const Net::Address& address,
                                                   const bool success)> cb) { m_hostConnectCallback = cb; }

private:
    std::unique_ptr<Net::Socket> m_socket;
    std::unique_ptr<Net::ReadStream> m_readStream;
    std::unique_ptr<Net::WriteStream> m_writeStream;
    unsigned char* m_readBuffer;
    unsigned char* m_writeBuffer;
    std::function<void()> m_connectCallback;
    std::function<void()> m_disconnectCallback;
    std::function<void(const std::string&)> m_infoCallback;
    std::function<void(const std::vector<Net::Transport::LobbyEntry>& hosts,
                       const uint32_t startRange,
                       const uint32_t endRange)> m_hostListCallback;
    std::function<void(const Net::Address& address,
                       const bool success)> m_clientConnectCallback;
    std::function<void(const Net::Address& address,
                       const bool success)> m_hostConnectCallback;
    Mode m_mode;
    State m_state;
    Net::Port m_port;
    Net::ProtocolID m_protocolID;
    float m_timeout;
    float m_timeSinceUpdateReceived;
    float m_timeSinceUpdateSent;
    Net::Address m_serverAddress;

    void readSocketData();
    void processReceivedData(const Net::Address& sender,
                             const int receivedBytes);
    void updateConnection(const float deltaTime);
    bool validateDataHeader(const int receivedBytes);
    void writeHeaderData(const int dataBytes);
    void sendMessage(const std::shared_ptr<Net::Message>& message);
    void sendPing(const bool isPong);
    void onGameListResponse();
    void onClientConnectionResponse();
    void onHostConnectionResponse();
    void onInfo(const std::string& info);
};
}
#endif /* MasterServerConnection_h */
