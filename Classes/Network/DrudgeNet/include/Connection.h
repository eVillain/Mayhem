#ifndef NET_CONNECTION_H
#define NET_CONNECTION_H

#include "Socket.h"
#include "Network/DrudgeNet/include/DataTypes.h"
#include <functional>

namespace Net
{
    class Connection
    {
    public:
        typedef std::function<void(void)> StartCallback;
        typedef std::function<void(void)> StopCallback;
        typedef std::function<void(const Address& sender)> ConnectCallback;
        typedef std::function<void(void)> DisconnectCallback;

        enum Mode
        {
            None,
            Client,
            Server
        };
        
        Connection(ProtocolID protocolId, float timeout);
        
        virtual ~Connection();
        
        bool start(Port port);
        void stop();
        
        bool isRunning() const { return m_running; }
        
        void listen();
        
        void connect(const Address& address);
        
        bool isConnecting() const { return m_state == Connecting; }
        bool connectFailed() const { return m_state == ConnectFail; }
        bool isConnected() const { return m_state == Connected; }
        bool isListening() const { return m_state == Listening; }
        
        Mode getMode() const { return m_mode; }
        
        virtual void update(const float deltaTime);
        
        virtual bool sendPacket(const unsigned char data[], int size );
        virtual int receivePacket(unsigned char data[], int size );
        
        virtual int getHeaderSize() const { return 4; }
        
        void setCallbacks(StartCallback startCallback,
                          StopCallback stopCallback,
                          ConnectCallback connectCallback,
                          DisconnectCallback disconnectCallback);
    protected:
        
        virtual void onStart();
        virtual void onStop();
        virtual void onConnect(const Address& address);
        virtual void onDisconnect();
        
    private:
        
        void clearData();
        
        enum State
        {
            Disconnected,
            Listening,
            Connecting,
            ConnectFail,
            Connected
        };
        
        uint32_t m_protocolId;
        float m_timeout;
        
        bool m_running;
        Mode m_mode;
        State m_state;
        Socket m_socket;
        float m_timeoutAccumulator;
        Address m_address;
        
        StartCallback m_startCallback;
        StopCallback m_stopCallback;
        ConnectCallback m_connectCallback;
        DisconnectCallback m_disconnectCallback;
    };
}


#endif /* Connection_hpp */
