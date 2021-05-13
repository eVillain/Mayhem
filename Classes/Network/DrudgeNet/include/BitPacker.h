#ifndef NET_BITPACKER_H
#define NET_BITPACKER_H

#include <cstdint>

namespace Net
{
    // bitpacker class
    //  + read and write non-8 multiples of bits efficiently
    class BitPacker
    {
    public:
        
        enum Mode
        {
            Read,
            Write
        };
        
        BitPacker(Mode mode, void* buffer, int32_t bytes);
        
        void WriteBits(uint32_t value, int32_t bits = 32);
        void WriteBits2(uint32_t value, uint32_t bits = 32);
        void WriteBits(uint64_t value, int32_t bits = 64);
        void ReadBits(uint32_t& value, int32_t bits = 32);
        void ReadBits2(uint32_t& value, uint32_t bits = 32);
        void ReadBits(uint64_t& value, int32_t bits = 64);
        
        void Clear();
        
        void* GetData();
        
        int32_t GetBits() const;
        
        int32_t GetBytes() const;
        
        int32_t BitsRemaining() const;
        
        Mode GetMode() const;
        
        bool IsValid() const;

    private:
        int32_t bit_index;
        unsigned char* ptr;
        unsigned char* buffer;
        int32_t bytes;
        Mode mode;
    };
}

#endif /* NET_BITPACKER_H */
