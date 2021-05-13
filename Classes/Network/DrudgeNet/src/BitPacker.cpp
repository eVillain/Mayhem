#include "BitPacker.h"
#include <cassert>
#include <algorithm>

namespace Net
{
    BitPacker::BitPacker(Mode mode, void* buffer, int32_t bytes)
    {
        assert( bytes >= 0 );
        this->mode = mode;
        this->buffer = (unsigned char*)buffer;
        this->ptr = (unsigned char*)buffer;
        this->bytes = bytes;
        bit_index = 0;
        if (mode == Write)
        {
            memset(buffer, 0, bytes);
        }
    }
    
    void BitPacker::WriteBits(uint32_t value, int32_t bits)
    {
        assert(ptr);
        assert(buffer);
        assert(bits > 0);
        assert(bits <= 32);
        assert(mode == Write);
        if (bits < 32)
        {
            const int32_t mask = (1 << bits) - 1;
            value &= mask;
        }
        do
        {
            assert(ptr - buffer < bytes);
            *ptr |= (uint8_t)(value << bit_index);
            assert(bit_index < 8);
            const int32_t bits_written = std::min( bits, 8 - bit_index);
            assert(bits_written > 0);
            assert(bits_written <= 8);
            bit_index += bits_written;
            if (bit_index >= 8)
            {
                ptr++;
                bit_index = 0;
                value >>= bits_written;
            }
            bits -= bits_written;
            assert(bits >= 0);
            assert(bits <= 32);
        }
        while (bits > 0);
    }

    void
    BitPacker::WriteBits2(uint32_t value, uint32_t bits/* = 32*/)
    {
      if (bits < 32)
      {
        const uint32_t mask = (1 << bits) - 1;
        value &= mask;
      }
      do
      {

        *this->ptr |= (uint8_t)(value << bit_index);

        const uint32_t bitsWritten = std::min(bits, 8 - (uint32_t)bit_index);

        bit_index += bitsWritten;

        if (bit_index >= 8)
        {
          this->ptr++;
          bit_index = 0;
          value >>= bitsWritten;
        }
        bits -= bitsWritten;
      }
      while (bits > 0);
    }
    
    void BitPacker::WriteBits( uint64_t value, int32_t bits )
    {
        uint32_t x = (uint32_t)(value>>32);
        uint32_t y = (uint32_t)value;
        WriteBits(x);
        WriteBits(y);
    }
    
    void BitPacker::ReadBits( uint64_t & value, int32_t bits )
    {
        uint32_t x, y;
        ReadBits(x, 32);
        ReadBits(y, 32);
        value = ((uint64_t)x) << 32 | y;
    }
    
    void BitPacker::ReadBits(uint32_t& value, int32_t bits)
    {
        assert(ptr);
        assert(buffer);
        assert(bits > 0);
        assert(bits <= 32);
        assert(mode == Read);
        int32_t original_bits = bits;
        int32_t value_index = 0;
        value = 0;
        do
        {
            assert(ptr - buffer < bytes );
            assert(bits >= 0);
            assert(bits <= 32);
            int32_t bits_to_read = std::min(8 - bit_index, bits);
            assert(bits_to_read > 0);
            assert(bits_to_read <= 8);
            value |= (*ptr >> bit_index) << value_index;
            bits -= bits_to_read;
            bit_index += bits_to_read;
            value_index += bits_to_read;
            assert(value_index >= 0);
            assert(value_index <= 32);
            if (bit_index >= 8)
            {
                ptr++;
                bit_index = 0;
            }
        }
        while (bits > 0);
        if (original_bits < 32)
        {
            const uint32_t mask = (1 << original_bits) - 1;
            value &= mask;
        }
    }

    void
    BitPacker::ReadBits2(uint32_t& value, uint32_t bits/* = 32*/)
    {
      uint32_t originalBits = bits;
      uint32_t valueIndex = 0;
      value = 0;

      do
      {
        uint32_t bitsToRead = std::min(8 - (uint32_t)bit_index, bits);

        value |= (*this->ptr >> bit_index) << valueIndex;
        bits -= bitsToRead;
        bit_index += bitsToRead;
        valueIndex += bitsToRead;

        if (bit_index >= 8)
        {
          this->ptr++;
          bit_index = 0;
        }
      }
      while (bits > 0);

      if (originalBits < 32)
      {
        const uint32_t mask = (1 << originalBits) - 1;
        value &= mask;
      }
    }
    
    void BitPacker::Clear()
    {
        bit_index = 0;
        ptr = buffer;
    }
    
    void * BitPacker::GetData()
    {
        return buffer;
    }
    
    int32_t BitPacker::GetBits() const
    {
        return (int32_t)(ptr - buffer) * 8 + bit_index;
    }
    
    int32_t BitPacker::GetBytes() const
    {
        return (int32_t)(ptr - buffer) + (bit_index > 0 ? 1 : 0);
    }
    
    int32_t BitPacker::BitsRemaining() const
    {
        return (int32_t)(bytes * 8 - ((ptr - buffer) * 8 + bit_index));
    }
    
    BitPacker::Mode BitPacker::GetMode() const
    {
        return mode;
    }
    
    bool BitPacker::IsValid() const
    {
        return buffer != NULL;
    }
}
