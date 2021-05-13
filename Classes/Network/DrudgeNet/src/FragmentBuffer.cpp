#include "FragmentBuffer.h"
#include <algorithm>
#include <cassert>

namespace Net
{
    FragmentBuffer::FragmentBuffer(const int32_t maxFragmentSize)
    : m_maxFragmentSize(maxFragmentSize)
    , m_data(nullptr)
    , m_dataSize(0)
    , m_fragmentCount(0)
    {
    }

    FragmentBuffer::~FragmentBuffer()
    {
        clearData();
    }

    void FragmentBuffer::setupFragmentData(const unsigned char* data,
                                           const int32_t dataSize)
    {
        assert(dataSize > 0);
        assert(m_data == nullptr);
        assert(m_dataSize == 0);
        assert(m_fragments.empty());
        
        const int32_t fragmentCount = (dataSize + m_maxFragmentSize - 1) / m_maxFragmentSize;
        assert(fragmentCount < 256);

        m_fragmentCount = fragmentCount;
        m_data = (unsigned char*)malloc(dataSize);
        
        if (data)
        {
            memcpy(m_data, data, dataSize);
            
            int32_t bytesLeft = dataSize;
            
            for (PacketFragmentID fragmentID = 0; fragmentID < fragmentCount; fragmentID++)
            {
                int32_t fragSize = std::min(bytesLeft, m_maxFragmentSize);
                
                FragmentData fragmentData;
                fragmentData.data = &m_data[getDataOffset(fragmentID)];
                fragmentData.size = fragSize;
                m_fragments[fragmentID] = fragmentData;
                
                bytesLeft -= fragSize;
            }
        }
    }

    void FragmentBuffer::clearData()
    {
        if (m_data)
        {
            delete [] m_data;
            m_data = nullptr;
        }
        
        m_dataSize = 0;
        m_fragmentCount = 0;
        
        m_fragments.clear();
        m_sentFragments.clear();
    }
    
    const unsigned char* FragmentBuffer::getFragmentData(const PacketFragmentID fragmentID)
    {
        return m_fragments.at(fragmentID).data;
    }
    
    const int32_t FragmentBuffer::getFragmentSize(const PacketFragmentID fragmentID)
    {
        return m_fragments.at(fragmentID).size;
    }
    
    bool FragmentBuffer::onFragmentSent(const PacketFragmentID fragmentID,
                                        const PacketSequenceID packetID)
    {
        if (m_fragments.count(fragmentID))
        {
            return false;
        }
        
        if (m_sentFragments.count(packetID))
        {
            return false;
        }
        
        m_sentFragments[packetID] = fragmentID;
        
        return true;
    }
    
    bool FragmentBuffer::onPacketAcked(const PacketSequenceID packetID)
    {
        if (m_sentFragments.count(packetID) == 0)
        {
            // Acked packet did not contain fragment
            return false;
        }
        
        const PacketFragmentID ackedFragmentID = m_sentFragments.at(packetID);
        
        // Remove all packet IDs which contained the acked fragment
        for(auto it = m_sentFragments.begin(); it != m_sentFragments.end();)
        {
            if (it->second == ackedFragmentID)
            {
                m_sentFragments.erase(it++);
            }
            else
            {
                ++it;
            }
        }
        
        // Fragment was acked so it no longer needs to be sent
        m_fragments.erase(ackedFragmentID);
        
        if (m_fragments.empty())
        {
            // All fragments have been acked
            clearData();
        }
        
        return true;
    }
    
    bool FragmentBuffer::onFragmentReceived(const unsigned char* fragmentData,
                                            const int32_t size,
                                            PacketFragmentID fragmentID,
                                            uint8_t fragmentCount)
    {
        if (m_fragments.count(fragmentID))
        {
            return false;
        }
        
        if (m_fragmentCount != fragmentCount)
        {
            return false;
        }
        
        memcpy(&m_data[getDataOffset(fragmentID)], fragmentData, size);
        
        FragmentData data;
        data.data = &m_data[getDataOffset(fragmentID)];
        data.size = size;
        m_fragments[fragmentID] = data;
        
        return true;
    }
    
    bool FragmentBuffer::isComplete()
    {
        for (uint8_t i = 0; i < m_fragmentCount; i++)
        {
            if (m_fragments.count(i) == 0)
            {
                return false;
            }
        }
        return true;
    }
    
    const size_t FragmentBuffer::getDataOffset(const PacketFragmentID fragmentID) const
    {
        return fragmentID * m_maxFragmentSize;
    }
}

