#ifndef NET_MESSAGE_FACTORY_H
#define NET_MESSAGE_FACTORY_H

#include "Network/DrudgeNet/include/DataTypes.h"
#include "ReadStream.h"
#include <cstdint>
#include <memory>

namespace Net
{
    class Message;
    
    class MessageFactory
    {
    public:
        virtual std::shared_ptr<Message> create(const MessageType type) = 0;
        virtual std::shared_ptr<Message> create(ReadStream& stream) = 0;
        virtual uint32_t getNumTypes() const = 0;
    };
}

#endif /* NET_MESSAGE_FACTORY_H */
