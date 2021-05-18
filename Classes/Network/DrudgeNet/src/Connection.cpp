#include "Connection.h"
#include <cassert>
#include <stdio.h>
#include <algorithm>

namespace Net
{
    Connection::Connection(ProtocolID protocolId, float timeout)
    : m_protocolId(protocolId)
    , m_timeout(timeout)
    , m_running(false)
    , m_mode(None)
    , m_state(Disconnected)
    , m_timeoutAccumulator(0.f)
    , m_startCallback(nullptr)
    , m_stopCallback(nullptr)
    , m_connectCallback(nullptr)
    , m_disconnectCallback(nullptr)
    {
    }
    
    Connection::~Connection()
    {
        if (isRunning())
        {
            stop();
        }
    }
    
    bool Connection::start(Port port)
    {
        assert(!m_running);
        if (!m_socket.Open(port))
        {
            return false;
        }
        m_running = true;
        onStart();
        return true;
    }
    
    void Connection::stop()
    {
        assert(m_running);
        bool connected = isConnected();
        clearData();
        m_socket.Close();
        m_running = false;
        if (connected)
        {
            onDisconnect();
        }
        onStop();
    }
    
    void Connection::listen()
    {
        bool connected = isConnected();
        clearData();
        if (connected)
        {
            onDisconnect();
        }
        m_mode = Server;
        m_state = Listening;
    }
    
    void Connection::connect(const Address& address)
    {
        bool connected = isConnected();
        clearData();
        if (connected)
        {
            onDisconnect();
        }
        m_mode = Client;
        m_state = Connecting;
        m_address = address;
    }
    
    void Connection::update(const float deltaTime)
    {
        assert(m_running);
        m_timeoutAccumulator += deltaTime;
        if (m_timeoutAccumulator > m_timeout)
        {
            if (m_state == Connecting)
            {
                clearData();
                m_state = ConnectFail;
                onDisconnect();
            }
            else if (m_state == Connected)
            {
                clearData();
                if (m_state == Connecting)
                {
                    m_state = ConnectFail;
                }
                onDisconnect();
            }
        }
    }
    
    bool Connection::sendPacket(const unsigned char data[], int size)
    {
        assert(m_running);
        if (m_address.GetAddressIP4() == 0)
        {
            return false;
        }
        unsigned char* packet = new unsigned char[size+4];
        packet[0] = (unsigned char) (m_protocolId >> 24);
        packet[1] = (unsigned char) ((m_protocolId >> 16) & 0xFF);
        packet[2] = (unsigned char) ((m_protocolId >> 8) & 0xFF);
        packet[3] = (unsigned char) ((m_protocolId) & 0xFF);
        memcpy(&packet[4], data, size);
        bool res = m_socket.Send(m_address, packet, size + 4);
        delete [] packet;
        return res;
    }
    
    int Connection::receivePacket(unsigned char data[], int size)
    {
        assert(m_running);
        unsigned char* packet = new unsigned char[size+4];
        Address sender;
        int bytes_read = m_socket.Receive(sender, packet, size + 4);
        if (bytes_read == 0)
        {
            delete [] packet;
            return 0;
        }
        if (bytes_read <= 4)
        {
            delete [] packet;
            return 0;
        }
        if (packet[0] != (unsigned char) (m_protocolId >> 24) ||
            packet[1] != (unsigned char) ((m_protocolId >> 16) & 0xFF) ||
            packet[2] != (unsigned char) ((m_protocolId >> 8) & 0xFF) ||
            packet[3] != (unsigned char) (m_protocolId & 0xFF))
        {
            delete [] packet;
            return 0;
        }
        
        if (m_mode == Server && !isConnected())
        {
            m_state = Connected;
            m_address = sender;
            onConnect(sender);
        }
        
        if (sender == m_address)
        {
            if (m_mode == Client && m_state == Connecting)
            {
                m_state = Connected;
                onConnect(sender);
            }
            m_timeoutAccumulator = 0.0f;
            memcpy(data, &packet[4], bytes_read - 4);
            delete [] packet;
            return bytes_read - 4;
        }
        delete [] packet;
        return 0;
    }
    
    void Connection::clearData()
    {
        m_state = Disconnected;
        m_timeoutAccumulator = 0.0f;
        m_address = Address();
    }
    
    void Connection::onStart()
    {
        if (m_startCallback)
        {
            m_startCallback();
        }
    }
    
    void Connection::onStop()
    {
        if (m_stopCallback)
        {
            m_stopCallback();
        }
    }
    
    void Connection::onConnect(const Address& address)
    {
        if (m_connectCallback)
        {
            m_connectCallback(address);
        }
    }
    
    void Connection::onDisconnect()
    {
        if (m_disconnectCallback)
        {
            m_disconnectCallback();
        }
    }
}
