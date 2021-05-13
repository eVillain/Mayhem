#ifndef Projectile_h
#define Projectile_h

#include "Entity.h"

class Projectile : public Entity
{
public:
    Projectile(const uint16_t entityID,
               const EntityType& type);
    ~Projectile();
    
    float getSpawnTime() const { return m_spawnTime; }
    void setSpawnTime(const float time) { m_spawnTime = time; }

    uint8_t getOwnerID() const { return m_ownerID; }
    void setOwnerID(const uint8_t ownerID) { m_ownerID = ownerID; }

private:
    float m_spawnTime;
    uint8_t m_ownerID;
};


#endif /* Projectile_h */
