#ifndef SpawnPlayerEvent_h
#define SpawnPlayerEvent_h

#include "Core/Event.h"
#include "cocos2d.h"

class SpawnPlayerEvent : public Event
{
public:
    SpawnPlayerEvent(const uint16_t entityID,
                     const uint8_t playerID,
                     const cocos2d::Vec2& position,
                     const float rotation);
    
    virtual DescriptorType type() const { return descriptor; }

    static constexpr DescriptorType descriptor = "SpawnPlayer";
    
    const uint16_t getEntityID() const { return m_entityID; }
    const uint8_t getPlayerID() const { return m_playerID; }
    const cocos2d::Vec2& getPosition() const { return m_position; }
    const float getRotation() const { return m_rotation; }
    
private:
    const uint16_t m_entityID;
    const uint8_t m_playerID;
    const cocos2d::Vec2 m_position;
    const float m_rotation;
};

#endif /* SpawnPlayerEvent_h */
