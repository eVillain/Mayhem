#ifndef NET_CHANNEL_RELIABLE_H
#define NET_CHANNEL_RELIABLE_H

#include "Channel.h"

namespace Net
{
    class ChannelReliable : public Channel
    {
    public:
        ChannelReliable() : Channel(Channel_Reliable_Ordered) {}

        virtual ~ChannelReliable() {};
        
        bool SendPacket(NodeID nodeId,
                        const unsigned char data[],
                        int size) override;
        
        int ReceivePacket(NodeID& nodeId,
                          unsigned char data[],
                          int size) override;
    };
}

#endif /* NET_CHANNEL_RELIABLE_H */
