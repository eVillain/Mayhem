#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <cstdint>

namespace Net
{
    typedef uint32_t ProtocolID;
    typedef uint16_t Port;

    typedef int32_t ClientID;
    typedef int32_t NodeID;
    
    typedef uint32_t PacketSequenceID;
    typedef uint8_t PacketFragmentID;
    
    typedef uint8_t MessageType;
    typedef uint32_t MessageID;
    enum ClientPacketType { JoinRequest = 0, KeepAlive, ClientMessage };
    enum HostPacketType { ConnectionAccepted = 0, Update, HostMessage };
}


#endif /* DATA_TYPES_H */
