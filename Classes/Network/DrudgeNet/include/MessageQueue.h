#ifndef NET_MESSAGE_QUEUE_H
#define NET_MESSAGE_QUEUE_H

#include <cstdint>
#include <functional>
#include <map>
#include "Stream.h"
#include "Network/DrudgeNet/include/DataTypes.h"
#include <functional>
#include <memory>
#include <stdint.h>
#include <vector>

namespace Net
{
    class Message;
    
    class MessageQueue
    {
    public:
        enum Mode
        {
            UNRELIABLE = 0,
            RELIABLE
        };
        
        struct MessageData
        {
            std::shared_ptr<Message> message;
            uint32_t bitsRequired;
            int32_t timeLastSent;
        };
        
        MessageQueue(Mode mode);
        
        void clear();
        
        MessageID queueMessage(std::shared_ptr<Message>& message,
                               uint32_t bitsRequired);
        void removeMessage(const MessageID messageID);
        
        void onPacketSent(const PacketSequenceID packetID,
                          std::vector<MessageID>& messages);
        
        
        bool getNextMessage(const int32_t bitsAvailable,
                            MessageID& messageID,
                            MessageData& data);
        
        bool isEmpty() const { return m_messages.empty(); }
        std::map<MessageID, MessageData>& getMessages() { return m_messages; }

    private:
        Mode m_mode;
        std::map<MessageID, MessageData> m_messages;
        MessageID m_nextMessageID;
    };
}

#endif /* NET_MESSAGE_QUEUE_H */
