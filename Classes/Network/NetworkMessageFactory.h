#ifndef NetworkMessageFactory_h
#define NetworkMessageFactory_h

#include "MessageFactory.h"
#include "NetworkMessages.h"
#include <functional>

class NetworkMessageFactory : public Net::MessageFactory
{
public:
    NetworkMessageFactory() : m_dataCallback(nullptr) {}
    std::shared_ptr<Net::Message> create(const Net::MessageType type) override;
    std::shared_ptr<Net::Message> create(Net::ReadStream& stream) override;
    uint32_t getNumTypes() const override;
    
    void setDeltaDataCallback(std::function<const SnapshotData&(const uint32_t)> dataCallback);

    static const std::string getNameForType(const Net::MessageType type);
private:
    std::function<const SnapshotData&(const uint32_t)> m_dataCallback;
};

#endif /* NetworkMessageFactory_h */
