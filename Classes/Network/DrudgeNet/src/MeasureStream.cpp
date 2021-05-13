#include "MeasureStream.h"
#include <cassert>
#include <cmath>
#include <algorithm>

namespace Net
{    
    MeasureStream::MeasureStream(int32_t bytes)
    : Stream(Mode::Read, nullptr, bytes, nullptr, 0)
    , m_bytes(bytes)
    , m_bitsProcessed(0)
    {
    }
    
    bool MeasureStream::SerializeBoolean(bool& value)
    {
        m_bitsProcessed += 1;
        return true;
    }
    
    bool MeasureStream::SerializeByte(int8_t& value, int8_t min, int8_t max)
    {
        m_bitsProcessed += BitsRequired(min, max);
        return true;
    }
    
    bool MeasureStream::SerializeByte(uint8_t& value, uint8_t min, uint8_t max )
    {
        m_bitsProcessed += BitsRequired(min, max);
        return true;
    }
    
    bool MeasureStream::SerializeShort(int16_t& value, int16_t min, int16_t max)
    {
        m_bitsProcessed += BitsRequired(min, max);
        return true;
    }
    
    bool MeasureStream::SerializeShort(uint16_t& value, uint16_t min, uint16_t max)
    {
        m_bitsProcessed += BitsRequired(min, max);
        return true;
    }
    
    bool MeasureStream::SerializeInteger(int32_t& value, int32_t min, int32_t max)
    {
        m_bitsProcessed += BitsRequired((min + 2147483646), (max + 2147483646));
        return true;
    }
    
    bool MeasureStream::SerializeInteger(uint32_t& value, uint32_t min, uint32_t max)
    {
        m_bitsProcessed += BitsRequired(min, max);
        return true;
    }
    
    bool MeasureStream::SerializeFloat(float& value)
    {
        m_bitsProcessed += 32;
        return true;
    }
    
    bool MeasureStream::SerializeFloatCompressed(float& value, float min, float max, float res)
    {
        const float delta = max - min;
        const float values = delta / res;
        const uint32_t maxIntegerValue = (uint32_t)std::ceil(values);
        m_bitsProcessed += BitsRequired(0, maxIntegerValue);
        return true;
    }

    bool MeasureStream::SerializeDouble(double & value)
    {
        m_bitsProcessed += 64;
        return true;
    }
    
    bool MeasureStream::SerializeDoubleCompressed(double& value, double min, double max, double res)
    {
        const double delta = max - min;
        const double values = delta / res;
        const uint32_t maxIntegerValue = (uint32_t)std::ceil(values);
        m_bitsProcessed += BitsRequired(0, maxIntegerValue);
        return true;
    }
    
    bool MeasureStream::SerializeBits(uint32_t& value, int32_t bits)
    {
        m_bitsProcessed += bits;
        return true;
    }
    
    bool MeasureStream::SerializeBits(uint64_t& value, int32_t bits)
    {
        m_bitsProcessed += bits;
        return true;
    }

    bool MeasureStream::SerializeString(std::string& value)
    {
        const uint8_t textLength = value.length();
        m_bitsProcessed = textLength * 8;
        return true;
    }
    
    bool MeasureStream::Checkpoint()
    {
        m_bitsProcessed += 32;
        return true;
    }

void MeasureStream::Clear()
{
    m_bitsProcessed = 0;
}

bool MeasureStream::IsReading() const
{
    return true;
}

bool MeasureStream::IsWriting() const
{
    return false;
}

bool MeasureStream::IsMeasuring() const
{
    return true;
}

int32_t MeasureStream::GetBitsProcessed() const
{
    return m_bitsProcessed;
}

int32_t MeasureStream::GetBitsRemaining() const
{
    return (m_bytes * 8) - m_bitsProcessed;
}

int32_t MeasureStream::GetDataBytes() const
{
    return m_bytes;
}

int32_t MeasureStream::GetJournalBytes() const
{
    return 0;
}
}
