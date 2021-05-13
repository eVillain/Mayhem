#include "MessageQueue.h"

namespace Net
{
    MessageQueue::MessageQueue(Mode mode)
    : m_mode(mode)
    , m_nextMessageID(0)
    {
    }

    void MessageQueue::clear()
    {
        m_messages.clear();
    }
    
    MessageID MessageQueue::queueMessage(std::shared_ptr<Message>& message,
                                         uint32_t bitsRequired)
    {
        MessageData data;
        data.message = message;
        data.bitsRequired = bitsRequired;
        data.timeLastSent = -1;
        
        MessageID msgID = m_nextMessageID;
        m_nextMessageID++;

        m_messages[msgID] = data;
        return msgID;
    }

    void MessageQueue::removeMessage(const MessageID messageID)
    {
        if (m_messages.find(messageID) != m_messages.end())
        {
            m_messages.erase(messageID);
        }
    }
    
    void MessageQueue::onPacketSent(const PacketSequenceID packetID,
                                    std::vector<MessageID>& messages)
    {
        if (m_mode == UNRELIABLE)
        {
            return;
        }
    }
    
    bool MessageQueue::getNextMessage(const int32_t bitsAvailable,
                                      MessageID& messageID,
                                      MessageData& data)
    {
        if (m_messages.empty())
        {
            return false;
        }
        
        auto pair = *m_messages.begin();
        
        if (pair.second.bitsRequired < bitsAvailable)
        {
            messageID = pair.first;
            data = pair.second;
            return true;
        }
        else
        {
            return false;
        }
        
        return false;
    }
}

