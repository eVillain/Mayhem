#ifndef NET_DATA_CONSTANTS_H
#define NET_DATA_CONSTANTS_H

#include <cstdint>

namespace Net
{
    static constexpr int32_t MAXIMUM_TRANSMISSION_UNIT_BYTES = 1200;
    static constexpr int32_t MAXIMUM_PACKET_BITS = MAXIMUM_TRANSMISSION_UNIT_BYTES * 8;
    static constexpr int32_t MAXIMUM_PACKET_FRAGMENTS = 256;
    static constexpr int32_t BUFFER_SIZE_BYTES = MAXIMUM_PACKET_FRAGMENTS * MAXIMUM_TRANSMISSION_UNIT_BYTES;
}


#endif /* NET_DATA_CONSTANTS_H */
