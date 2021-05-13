#include "Stream.h"
#include <cassert>
#include <cmath>
#include <algorithm>

namespace Net
{    
    Stream::Stream(Mode mode,
                   void* buffer,
                   int32_t bytes,
                   void* journal_buffer,
                   int32_t journal_bytes)
    : bitpacker(mode == Write ? BitPacker::Write : BitPacker::Read, buffer, bytes)
    , journal(mode == Write ? BitPacker::Write : BitPacker::Read, journal_buffer, journal_bytes)
    {
    }
    
    void Stream::Clear()
    {
        bitpacker.Clear();
    }
    
    bool Stream::IsReading() const
    {
        return bitpacker.GetMode() == BitPacker::Read;
    }
    
    bool Stream::IsWriting() const
    {
        return bitpacker.GetMode() == BitPacker::Write;
    }

    bool Stream::IsMeasuring() const
    {
        return false;
    }
    
    int32_t Stream::GetBitsProcessed() const
    {
        return bitpacker.GetBits();
    }
    
    int32_t Stream::GetBitsRemaining() const
    {
        return bitpacker.BitsRemaining();
    }
    
    int32_t Stream::BitsRequired(uint32_t minimum, uint32_t maximum)
    {
        assert(maximum > minimum);
        assert(maximum >= 1);
        if (maximum - minimum >= 0x7FFFFFF)
            return 32;
        return BitsRequired(maximum - minimum + 1);
    }
    
    int32_t Stream::BitsRequired(uint32_t distinctValues)
    {
        assert(distinctValues > 1);
        uint32_t maximumValue = distinctValues - 1;
        for (int index = 0; index < 32; ++index)
        {
            if ((maximumValue & ~1) == 0)
                return index + 1;
            maximumValue >>= 1;
        }
        return 32;
    }
    
    int32_t Stream::GetDataBytes() const
    {
        return bitpacker.GetBytes();
    }
    
    int32_t Stream::GetJournalBytes() const
    {
        return journal.GetBytes();
    }
    
    void Stream::DumpJournal()
    {
        if (journal.IsValid())
        {
            printf("-----------------------------\n");
            printf("dump journal:\n");
            BitPacker reader(BitPacker::Read, journal.GetData(), journal.GetBytes());
            while (reader.BitsRemaining() > 6)
            {
                unsigned int token = 0;
                reader.ReadBits(token, 6);
                if (token == 0)
                    break;
                if (token == 1)
                    printf(" (checkpoint)\n");
                else
                    printf(" + %d bits\n", token - 2);
            }
            printf("-----------------------------\n");
        }
        else
            printf("no journal exists!\n");
    }
}
