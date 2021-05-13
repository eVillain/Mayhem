#include "WriteStream.h"
#include <cassert>
#include <cmath>
#include <algorithm>

namespace Net
{    
    WriteStream::WriteStream(void* buffer,
                   int32_t bytes,
                   void* journal_buffer,
                   int32_t journal_bytes)
    : Stream(Mode::Write, buffer, bytes, journal_buffer, journal_bytes)
    {
    }
    
    bool WriteStream::SerializeBoolean(bool& value)
    {
        uint32_t tmp = (uint32_t)value;
        bool result = SerializeBits(tmp, 1);
        value = (bool)tmp;
        return result;
    }
    
    bool WriteStream::SerializeByte(int8_t& value, int8_t min, int8_t max)
    {
        uint32_t tmp = (uint32_t)(value + 127);
        bool result = SerializeInteger(tmp, (uint32_t)(min + 127), (max + 127));
        value = ((int8_t)tmp) - 127;
        return result;
    }
    
    bool WriteStream::SerializeByte(uint8_t& value, uint8_t min, uint8_t max )
    {
        uint32_t tmp = (uint32_t)value;
        bool result = SerializeInteger(tmp, min, max);
        value = (uint8_t)tmp;
        return result;
    }
    
    bool WriteStream::SerializeShort(int16_t& value, int16_t min, int16_t max)
    {
        uint32_t tmp = (uint32_t)(value + 32767);
        bool result = SerializeInteger(tmp, (uint32_t)(min + 32767), (max + 32767));
        value = ((int16_t)tmp) - 32767;
        return result;
    }
    
    bool WriteStream::SerializeShort(uint16_t& value, uint16_t min, uint16_t max)
    {
        uint32_t tmp = (uint32_t)value;
        bool result = SerializeInteger(tmp, min, max);
        value = (uint16_t)tmp;
        return result;
    }
    
    bool WriteStream::SerializeInteger(int32_t& value, int32_t min, int32_t max)
    {
        uint32_t tmp = (uint32_t)(value + 2147483646);
        bool result = SerializeInteger(tmp, (uint32_t)(min + 2147483646), (max + 2147483646));
        value = ((int32_t)tmp) - 2147483646;
        return result;
    }
    
    bool WriteStream::SerializeInteger(uint32_t& value, uint32_t min, uint32_t max)
    {
        assert(min < max);
        assert(value >= min);
        assert(value <= max);
        
        const int32_t bits_required = BitsRequired(min, max);
        uint32_t bits = value - min;
        bool result = SerializeBits(bits, bits_required);
        return result;
    }
    
    bool WriteStream::SerializeFloat(float& value)
    {
        union FloatInt
        {
            uint32_t i;
            float f;
        };
        
        FloatInt floatInt;
        floatInt.f = value;
        return SerializeBits(floatInt.i, 32);
    }
    
    bool WriteStream::SerializeFloatCompressed(float& value, float min, float max, float res)
    {
        const float delta = max - min;
        const float values = delta / res;
        const uint32_t maxIntegerValue = (uint32_t)std::ceil(values);
        const int32_t bits = BitsRequired(0, maxIntegerValue);
        uint32_t integerValue = 0;
        
        float normalizedValue = std::min(1.0f, std::max((value - min) / delta, 0.0f));
        integerValue = (uint32_t)std::floor(normalizedValue * maxIntegerValue + 0.5f);
        
        if (!SerializeBits(integerValue, bits))
        {
            return false;
        }
        return true;
    }

    bool WriteStream::SerializeDouble(double & value)
    {
        union DoubleInt64
        {
            uint64_t i;
            double d;
        };
        DoubleInt64 doubleInt;
        doubleInt.d = value;
        return SerializeBits(doubleInt.i, 64);
    }
    
    bool WriteStream::SerializeDoubleCompressed(double& value, double min, double max, double res)
    {
        const double delta = max - min;
        const double values = delta / res;
        const uint32_t maxIntegerValue = (uint32_t)std::ceil(values);
        const int32_t bits = BitsRequired(0, maxIntegerValue);
        uint32_t integerValue = 0;

        double normalizedValue = std::min(1.0, std::max((value - min) / delta, 0.0));
        integerValue = (uint32_t)std::floor(normalizedValue * maxIntegerValue + 0.5);

        if (!SerializeBits(integerValue, bits))
        {
            return false;
        }
        return true;
    }
    
    bool WriteStream::SerializeBits(uint32_t& value, int32_t bits)
    {
        assert(bits >= 1);
        assert(bits <= 32);
        if (bitpacker.BitsRemaining() < bits)
            return false;
        if (journal.IsValid())
        {
            uint32_t token = 2 + bits;		// note: 0 = end, 1 = checkpoint, [2,34] = n - 2 bits written
            journal.WriteBits(token, 6);
        }
        
        bitpacker.WriteBits(value, bits);
        
        return true;
    }
    
    bool WriteStream::SerializeBits(uint64_t & value, int32_t bits)
    {
        assert(bits >= 1);
        assert(bits <= 64);
        if (bitpacker.BitsRemaining() < bits)
            return false;
        if (journal.IsValid())
        {
            uint32_t token = 2 + bits;		// note: 0 = end, 1 = checkpoint, [2,66] = n - 2 bits written
            journal.WriteBits(token, 6);
        }
        
        bitpacker.WriteBits(value, bits);
        
        return true;
    }

    bool WriteStream::SerializeString(std::string& value)
    {
        assert(value.length() < 256);
        uint8_t textLength = value.length();
        const int32_t bits = textLength * 8;
        if (bitpacker.BitsRemaining() < bits)
            return false;
        
        SerializeByte(textLength);
        for (int i = 0; i < textLength; i++)
        {
            unsigned char c = value.c_str()[i];
            SerializeByte(c);
        }
                
        return true;
    }
    
    bool WriteStream::Checkpoint()
    {
        if (journal.IsValid())
        {
            uint32_t token = 1;		// note: 0 = end, 1 = checkpoint, [2,34] = n - 2 bits written
            journal.WriteBits(token, 6);
        }
        uint32_t magic = 0x12345678;
        uint32_t value = magic;
        if (bitpacker.BitsRemaining() < 32)
        {
            printf("not enough bits remaining for checkpoint\n");
            return false;
        }

        bitpacker.WriteBits(value, 32);
        
        if (value != magic)
        {
            printf("checkpoint failed!\n");
            return false;
        }
        return true;
    }
}
