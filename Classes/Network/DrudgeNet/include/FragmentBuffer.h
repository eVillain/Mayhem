#ifndef FRAGMENT_BUFFER_H
#define FRAGMENT_BUFFER_H

#include "Network/DrudgeNet/include/DataTypes.h"
#include <cstdint>
#include <map>

namespace Net
{
    class FragmentBuffer
    {
    public:
        FragmentBuffer(const int32_t maxFragmentSize);
        ~FragmentBuffer();

        // pass nullptr to set up for receiving data
        void setupFragmentData(const unsigned char* data,
                               const int32_t dataSize);
        void clearData();
        
        // Send fragment interface
        const unsigned char* getFragmentData(const PacketFragmentID fragmentID);
        const int32_t getFragmentSize(const PacketFragmentID fragmentID);
        
        const unsigned char* getData() const { return m_data; }
        const int32_t getDataSize() const { return m_dataSize; }
        const int32_t getFragmentCount() const { return m_fragmentCount; }
        
        bool onFragmentSent(const PacketFragmentID fragmentID,
                            const PacketSequenceID packetID);
        bool onPacketAcked(const PacketSequenceID packetID);

        // Receive fragment interface
        bool onFragmentReceived(const unsigned char* fragmentData,
                                const int32_t size,
                                PacketFragmentID fragmentID,
                                PacketFragmentID fragmentCount);
        
        bool isComplete();

    private:
        struct FragmentData
        {
            int32_t size;
            unsigned char* data;
        };
        
        const int32_t m_maxFragmentSize;
        unsigned char* m_data;
        int32_t m_dataSize;
        int32_t m_fragmentCount;

        std::map<PacketFragmentID, FragmentData> m_fragments;
        std::map<PacketSequenceID, PacketFragmentID> m_sentFragments;
        
        const size_t getDataOffset(const uint8_t fragmentID) const;
    };

}

#endif /* FRAGMENT_BUFFER_H */
