#ifndef DestroyEntityEvent_h
#define DestroyEntityEvent_h

#include "Core/Event.h"
#include "Entity.h"

class DestroyEntityEvent : public Event
{
public:
    DestroyEntityEvent(const uint16_t entityID);
    
    virtual DescriptorType type() const { return descriptor; }

    static constexpr DescriptorType descriptor = "DestroyEntity";

    const uint16_t getEntityID() const { return m_entityID; }
private:
    const uint16_t m_entityID;
};

#endif /* DestroyEntityEvent_h */
