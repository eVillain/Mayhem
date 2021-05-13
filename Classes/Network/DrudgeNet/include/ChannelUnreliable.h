#ifndef NET_CHANNEL_UNRELIABLE_H
#define NET_CHANNEL_UNRELIABLE_H

#include "Channel.h"

namespace Net
{
    class ChannelUnreliable : public Channel
    {
    public:
        ChannelUnreliable();
        
        virtual ~ChannelUnreliable();
        
        bool SendPacket(NodeID nodeId,
                        const unsigned char data[],
                        int size) override;
        
        int ReceivePacket(NodeID& nodeId,
                          unsigned char data[],
                          int size) override;
    };
}

#endif /* NET_CHANNEL_UNRELIABLE_H */
