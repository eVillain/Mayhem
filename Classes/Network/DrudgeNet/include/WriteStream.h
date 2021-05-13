#ifndef NET_WRITE_STREAM_H
#define NET_WRITE_STREAM_H

#include "Stream.h"

namespace Net
{
    class WriteStream : public Stream
    {
    public:
        WriteStream(void* buffer,
                    int32_t bytes,
                    void* journal_buffer = nullptr,
                    int32_t journal_bytes = 0);
        
        bool SerializeBoolean(bool & value) override;
        bool SerializeByte(int8_t& value, int8_t min = -128, int8_t max = +127 ) override;
        bool SerializeByte(uint8_t& value, uint8_t min = 0, uint8_t max = 255 ) override;
        bool SerializeShort(int16_t& value, int16_t min = -32768, int16_t max = +32767) override;
        bool SerializeShort(uint16_t& value, uint16_t min = 0, uint16_t max = 0xFFFF ) override;
        bool SerializeInteger(int32_t& value, int32_t min = -2147483646, int32_t max = +2147483647 ) override;
        bool SerializeInteger(uint32_t& value, uint32_t min = 0, uint32_t max = 0xFFFFFFFF ) override;
        bool SerializeFloat(float& value) override;
        bool SerializeFloatCompressed(float& value, float min, float max, float res) override;
        bool SerializeDouble(double& value) override;
        bool SerializeDoubleCompressed(double& value, double min, double max, double res) override;
        bool SerializeBits(uint32_t& value, int32_t bits) override;
        bool SerializeBits(uint64_t& value, int32_t bits) override;
        bool SerializeString(std::string& value) override;

        bool Checkpoint() override;
    };
}

#endif /* NET_WRITE_STREAM_H */
