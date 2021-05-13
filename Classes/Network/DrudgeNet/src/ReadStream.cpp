#include "ReadStream.h"
#include <cassert>
#include <cmath>
#include <algorithm>

namespace Net
{    
    ReadStream::ReadStream(void* buffer,
                   int32_t bytes,
                   void* journal_buffer,
                   int32_t journal_bytes)
    : Stream(Mode::Read, buffer, bytes, journal_buffer, journal_bytes)
    {
    }
    
    bool ReadStream::SerializeBoolean(bool& value)
    {
        uint32_t tmp = (uint32_t)value;
        bool result = SerializeBits(tmp, 1);
        value = (bool)tmp;
        return result;
    }
    
    bool ReadStream::SerializeByte(int8_t& value, int8_t min, int8_t max)
    {
        uint32_t tmp = (uint32_t)(value + 127);
        bool result = SerializeInteger(tmp, (uint32_t)(min + 127), (max + 127));
        value = ((int8_t)tmp) - 127;
        return result;
    }
    
    bool ReadStream::SerializeByte(uint8_t& value, uint8_t min, uint8_t max )
    {
        uint32_t tmp = (uint32_t)value;
        bool result = SerializeInteger(tmp, min, max);
        value = (uint8_t)tmp;
        return result;
    }
    
    bool ReadStream::SerializeShort(int16_t& value, int16_t min, int16_t max)
    {
        uint32_t tmp = (uint32_t)(value + 32767);
        bool result = SerializeInteger(tmp, (uint32_t)(min + 32767), (max + 32767));
        value = ((int16_t)tmp) - 32767;
        return result;
    }
    
    bool ReadStream::SerializeShort(uint16_t& value, uint16_t min, uint16_t max)
    {
        uint32_t tmp = (uint32_t)value;
        bool result = SerializeInteger(tmp, min, max);
        value = (uint16_t)tmp;
        return result;
    }
    
    bool ReadStream::SerializeInteger(int32_t& value, int32_t min, int32_t max)
    {
        uint32_t tmp = (uint32_t)(value + 2147483646);
        bool result = SerializeInteger(tmp, (uint32_t)(min + 2147483646), (max + 2147483646));
        value = ((int32_t)tmp) - 2147483646;
        return result;
    }
    
    bool ReadStream::SerializeInteger(uint32_t& value, uint32_t min, uint32_t max)
    {
        assert(min < max);
        const int32_t bits_required = BitsRequired(min, max);
        uint32_t bits = value - min;
        bool result = SerializeBits(bits, bits_required);

        value = bits + min;
        assert(value >= min);
        assert(value <= max);
        
        return result;
    }
    
    bool ReadStream::SerializeFloat(float& value)
    {
        union FloatInt
        {
            uint32_t i;
            float f;
        };
        
        FloatInt floatInt;
        if (!SerializeBits(floatInt.i, 32))
            return false;
        value = floatInt.f;
        return true;
    }
    
    bool ReadStream::SerializeFloatCompressed(float& value, float min, float max, float res)
    {
        const float delta = max - min;
        const float values = delta / res;
        const uint32_t maxIntegerValue = (uint32_t)std::ceil(values);
        const int32_t bits = BitsRequired(0, maxIntegerValue);
        uint32_t integerValue = 0;
        if (!SerializeBits(integerValue, bits))
        {
            return false;
        }

        const float normalizedValue = integerValue / float(maxIntegerValue);
        value = normalizedValue * delta + min;

        return true;
    }

    bool ReadStream::SerializeDouble(double & value)
    {
        union DoubleInt64
        {
            uint64_t i;
            double d;
        };
        
        DoubleInt64 doubleInt;
        if (!SerializeBits(doubleInt.i, 64))
            return false;
        value = doubleInt.d;
        return true;
    }
    
    bool ReadStream::SerializeDoubleCompressed(double& value, double min, double max, double res)
    {
        const double delta = max - min;
        const double values = delta / res;
        const uint32_t maxIntegerValue = (uint32_t)std::ceil(values);
        const int32_t bits = BitsRequired(0, maxIntegerValue);
        uint32_t integerValue = 0;
        if (!SerializeBits(integerValue, bits))
        {
            return false;
        }
        
        const double normalizedValue = integerValue / double(maxIntegerValue);
        value = normalizedValue * delta + min;
        
        return true;
    }
    
    bool ReadStream::SerializeBits(uint32_t& value, int32_t bits)
    {
        assert(bits >= 1);
        assert(bits <= 32);
        if (bitpacker.BitsRemaining() < bits)
            return false;
        if (journal.IsValid())
        {
            uint32_t token = 2 + bits;		// note: 0 = end, 1 = checkpoint, [2,34] = n - 2 bits written
            journal.ReadBits(token, 6);
            int32_t bits_written = token - 2;
            if (bits != bits_written)
            {
                printf("desync read/write: attempting to read %d bits when %d bits were written\n", bits, bits_written);
                return false;
            }
        }
        
        bitpacker.ReadBits(value, bits);
        
        return true;
    }
    
    bool ReadStream::SerializeBits(uint64_t & value, int32_t bits)
    {
        assert(bits >= 1);
        assert(bits <= 64);
        if (bitpacker.BitsRemaining() < bits)
            return false;
        if (journal.IsValid())
        {
            uint32_t token = 2 + bits;		// note: 0 = end, 1 = checkpoint, [2,66] = n - 2 bits written
            journal.ReadBits(token, 6);
            int bits_written = token - 2;
            if (bits != bits_written)
            {
                printf("desync read/write: attempting to read %d bits when %d bits were written\n", bits, bits_written);
                return false;
            }
        }
        
        bitpacker.ReadBits(value, bits);
        
        return true;
    }

    bool ReadStream::SerializeString(std::string& value)
    {
        uint8_t textLength = 0;
        if (!SerializeByte(textLength))
        {
            return false;
        }
        assert(textLength > 0);
        unsigned char* characters = new unsigned char[textLength + 1];
        for (int i = 0; i < textLength; i++)
        {
            if (!SerializeByte(characters[i]))
            {
                return false;
            }
        }
        characters[textLength] = '\0';
        value = (char*)characters;
		delete [] characters;

        return true;
    }
    
    bool ReadStream::Checkpoint()
    {
        if (journal.IsValid())
        {
            uint32_t token = 1;		// note: 0 = end, 1 = checkpoint, [2,34] = n - 2 bits written
            journal.ReadBits(token, 6);
            if (token != 1)
            {
                printf( "desync read/write: checkpoint not present in journal\n" );
                return false;
            }
        }
        uint32_t magic = 0x12345678;
        uint32_t value = magic;
        if (bitpacker.BitsRemaining() < 32)
        {
            printf("not enough bits remaining for checkpoint\n");
            return false;
        }
        
        bitpacker.ReadBits(value, 32);
        
        if (value != magic)
        {
            printf("checkpoint failed!\n");
            return false;
        }
        return true;
    }
}
