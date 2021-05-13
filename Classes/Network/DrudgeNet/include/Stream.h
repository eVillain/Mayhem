#ifndef NET_STREAM_H
#define NET_STREAM_H

#include "BitPacker.h"
#include <stdio.h>
#include <cstdint>
#include <string>

namespace Net
{
    // stream class
    //  + unifies read and write into a serialize operation
    //  + provides attribution of stream for debugging purposes
    class Stream
    {
    public:
        
        enum Mode
        {
            Read,
            Write
        };
        
        Stream(Mode mode,
               void* buffer,
               int32_t bytes,
               void* journal_buffer = NULL,
               int32_t journal_bytes = 0);
        
        virtual bool SerializeBoolean(bool & value) = 0;
        virtual bool SerializeByte(int8_t& value, int8_t min = -128, int8_t max = +127 ) = 0;
        virtual bool SerializeByte(uint8_t& value, uint8_t min = 0, uint8_t max = 255 ) = 0;
        virtual bool SerializeShort(int16_t& value, int16_t min = -32768, int16_t max = +32767) = 0;
        virtual bool SerializeShort(uint16_t& value, uint16_t min = 0, uint16_t max = 0xFFFF ) = 0;
        virtual bool SerializeInteger(int32_t& value, int32_t min = -2147483646, int32_t max = +2147483647 ) = 0;
        virtual bool SerializeInteger(uint32_t& value, uint32_t min = 0, uint32_t max = 0xFFFFFFFF ) = 0;
        virtual bool SerializeFloat(float& value) = 0;
        virtual bool SerializeFloatCompressed(float& value, float min, float max, float res) = 0;
        virtual bool SerializeDouble(double& value) = 0;
        virtual bool SerializeDoubleCompressed(double& value, double min, double max, double res) = 0;
        virtual bool SerializeBits(uint32_t& value, int32_t bits) = 0;
        virtual bool SerializeBits(uint64_t& value, int32_t bits) = 0;
        virtual bool SerializeString(std::string& value) = 0;

        virtual bool Checkpoint() = 0;

        virtual void Clear();
        
        virtual bool IsReading() const;
        virtual bool IsWriting() const;
        virtual bool IsMeasuring() const;
        
        virtual int32_t GetBitsProcessed() const;
        virtual int32_t GetBitsRemaining() const;
        virtual int32_t GetDataBytes() const;
        virtual int32_t GetJournalBytes() const;
        
        void DumpJournal();
        
        static int32_t BitsRequired(uint32_t minimum, uint32_t maximum);
        static int32_t BitsRequired(uint32_t distinctValues);
        
    protected:
        BitPacker bitpacker;
        BitPacker journal;
    };
}

#endif /* NET_STREAM_H */
