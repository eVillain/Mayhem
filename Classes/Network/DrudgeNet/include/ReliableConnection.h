#ifndef NET_RELIABLE_CONNECTION_H
#define NET_RELIABLE_CONNECTION_H

#include "Connection.h"
#include "ReliabilitySystem.h"
#include "Network/DrudgeNet/include/DataTypes.h"

// connection with reliability (seq/ack)

namespace Net
{
    class ReliableConnection : public Connection
    {
    public:
        
        ReliableConnection(ProtocolID protocolId, float timeout, uint32_t max_sequence = 0xFFFFFFFF);
        
        ~ReliableConnection();
        
        void update(const float deltaTime) override;
        
        bool sendPacket(const unsigned char data[], int size) override;

        int receivePacket(unsigned char data[], int size) override;
        
        int getHeaderSize() const override;
        
        ReliabilitySystem &getReliabilitySystem() { return m_reliabilitySystem; }

    protected:
        
        void writeInteger(unsigned char* data, uint32_t value);
        
        void writeHeader(unsigned char* header, uint32_t sequence, uint32_t ack, uint32_t ack_bits);
        
        void readInteger(const unsigned char* data, uint32_t& value);
        
        void readHeader(const unsigned char* header, uint32_t& sequence, uint32_t& ack, uint32_t& ack_bits);
        
        void onStop() override;
        
        void onDisconnect() override;
        
    private:
        
        void clearData();
        
        ReliabilitySystem m_reliabilitySystem;	// reliability system: manages sequence numbers and acks, tracks network stats etc.
    };

}

#endif /* ReliableConnection_h */
