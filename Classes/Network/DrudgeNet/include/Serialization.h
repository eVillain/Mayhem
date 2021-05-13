#ifndef NET_SERIALIZATION_H
#define NET_SERIALIZATION_H

namespace Net
{
    namespace Serialization
    {
        inline void WriteInteger(unsigned char* data, uint32_t value)
        {
            data[0] = (unsigned char) ( value >> 24 );
            data[1] = (unsigned char) ( ( value >> 16 ) & 0xFF );
            data[2] = (unsigned char) ( ( value >> 8 ) & 0xFF );
            data[3] = (unsigned char) ( value & 0xFF );
        }
        
        inline void ReadInteger(const unsigned char* data, uint32_t& value)
        {
            value = (( (uint32_t)data[0] << 24 ) |
                     ( (uint32_t)data[1] << 16 ) |
                     ( (uint32_t)data[2] << 8 )  |
                     ( (uint32_t)data[3] ) );
        }
        
        inline void WriteShort(unsigned char* data, uint16_t value)
        {
            data[0] = (unsigned char) ( ( value >> 8 ) & 0xFF );
            data[1] = (unsigned char) ( value & 0xFF );
        }
        
        inline void ReadShort(const unsigned char* data, uint16_t& value)
        {
            value = (( (uint32_t)data[0] << 8 )  |
                     ( (uint32_t)data[1] ) );
        }
    }
}

#endif /* NET_SERIALIZATION_H */
