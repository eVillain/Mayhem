#include "ReliableConnection.h"

namespace Net
{
    ReliableConnection::ReliableConnection(ProtocolID protocolId,
                                           float timeout,
                                           uint32_t max_sequence)
    : Connection(protocolId, timeout)
    , m_reliabilitySystem(max_sequence)
    {
        clearData();
    }
    
    ReliableConnection::~ReliableConnection()
    {
        if (isRunning())
        {
            stop();
        }
    }
    
    void ReliableConnection::update(const float deltaTime)
    {
        Connection::update(deltaTime);
        m_reliabilitySystem.Update(deltaTime);
    }
    
    bool ReliableConnection::sendPacket(const unsigned char data[], int size)
    {
        const int header = 12;
        unsigned char * packet = new unsigned char[header+size];
        uint32_t seq = m_reliabilitySystem.GetLocalSequence();
        uint32_t ack = m_reliabilitySystem.GetRemoteSequence();
        uint32_t ack_bits = m_reliabilitySystem.GenerateAckBits();
        writeHeader( packet, seq, ack, ack_bits );
        memcpy( packet + header, data, size );
        if (!Connection::sendPacket(packet, size + header))
        {
            return false;
        }
        m_reliabilitySystem.PacketSent(size);
        delete [] packet;
        return true;
    }
    
    int ReliableConnection::receivePacket(unsigned char data[], int size)
    {
        const int header = 12;
        if (size <= header)
        {
            return false;
        }
        unsigned char * packet = new unsigned char[header+size];
        int received_bytes = Connection::receivePacket( packet, size + header );
        if ( received_bytes == 0 )
        {
            delete [] packet;
            return false;
        }
        if ( received_bytes <= header )
        {
            delete [] packet;
            return false;
        }
        uint32_t packet_sequence = 0;
        uint32_t packet_ack = 0;
        uint32_t packet_ack_bits = 0;
        readHeader( packet, packet_sequence, packet_ack, packet_ack_bits );
        m_reliabilitySystem.PacketReceived( packet_sequence, received_bytes - header );
        m_reliabilitySystem.ProcessAck( packet_ack, packet_ack_bits );
        memcpy( data, packet + header, received_bytes - header );
        delete [] packet;
        return received_bytes - header;
    }

    
    int ReliableConnection::getHeaderSize() const
    {
        return Connection::getHeaderSize() + m_reliabilitySystem.GetHeaderSize();
    }
    
    void ReliableConnection::writeInteger(unsigned char* data, uint32_t value)
    {
        data[0] = (unsigned char) (value >> 24);
        data[1] = (unsigned char) ((value >> 16) & 0xFF);
        data[2] = (unsigned char) ((value >> 8) & 0xFF);
        data[3] = (unsigned char) (value & 0xFF);
    }
    
    void ReliableConnection::writeHeader(unsigned char* header, uint32_t sequence, uint32_t ack, uint32_t ack_bits)
    {
        writeInteger( header, sequence );
        writeInteger( header + 4, ack );
        writeInteger( header + 8, ack_bits );
    }
    
    void ReliableConnection::readInteger(const unsigned char* data, uint32_t& value)
    {
        value = (((uint32_t)data[0] << 24 ) | ( (uint32_t)data[1] << 16) |
                 ((uint32_t)data[2] << 8 )  | ( (uint32_t)data[3]));
    }
    
    void ReliableConnection::readHeader(const unsigned char* header, uint32_t& sequence, uint32_t& ack, uint32_t& ack_bits )
    {
        readInteger( header, sequence );
        readInteger( header + 4, ack );
        readInteger( header + 8, ack_bits );
    }
    
    void ReliableConnection::onStop()
    {
        Connection::onStop();
        clearData();
    }
    
    void ReliableConnection::onDisconnect()
    {
        Connection::onDisconnect();
        clearData();
    }
    
    void ReliableConnection::clearData()
    {        
        m_reliabilitySystem.Reset();
    }
}
