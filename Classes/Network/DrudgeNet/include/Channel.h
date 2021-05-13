#ifndef NET_CHANNEL_H
#define NET_CHANNEL_H

#include "Network/DrudgeNet/include/DataTypes.h"

namespace Net
{
    enum ChannelType
    {
        Channel_Reliable_Ordered,
        Channel_Unreliable_Unordered,
    };
    
    class Channel
    {
    public:
        virtual ~Channel() {};
        
        virtual bool SendPacket(NodeID nodeId,
                                const unsigned char data[],
                                int size) = 0;
        
        virtual int ReceivePacket(NodeID& nodeId,
                                  unsigned char data[],
                                  int size) = 0;
    protected:
        Channel(const ChannelType type) : m_type(type) {}
        
    private:
        const ChannelType m_type;
    };
}

#endif /* NET_CHANNEL_H */
