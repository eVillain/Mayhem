#ifndef NET_MESSAGE_H
#define NET_MESSAGE_H

#include "DataTypes.h"
#include "MeasureStream.h"
#include "ReadStream.h"
#include "Stream.h"
#include "WriteStream.h"
#include <cstdint>

namespace Net
{
    class WriteStream;
    class ReadStream;
    class MeasureStream;

    class Message
    {
    public:
        Message(MessageType type) : m_type(type) {}
        
        ~Message() {}
        
        virtual bool serialize(WriteStream& stream) = 0;
        
        virtual bool serialize(ReadStream& stream) = 0;
        
        virtual bool serialize(MeasureStream& stream) = 0;
        
        MessageType getType() const { return m_type; }
        
    private:
        MessageType m_type;
    };
}

#endif /* NET_MESSAGE_H */
