#include "ChannelUnreliable.h"

namespace Net
{
    ChannelUnreliable::ChannelUnreliable()
    : Channel(Channel_Unreliable_Unordered)
    {
        
    }
        
    ChannelUnreliable::~ChannelUnreliable()
    {
        
    }
        
    bool ChannelUnreliable::SendPacket(NodeID nodeId,
                                       const unsigned char data[],
                                       int size)
    {
        
        return true;
    }
        
    int ChannelUnreliable::ReceivePacket(NodeID& nodeId,
                                         unsigned char data[],
                                         int size)
    {
        return 0;
    }
}
