#include "Projectile.h"


Projectile::Projectile(const uint16_t entityID,
                       const EntityType& type)
: Entity(entityID, type)
, m_spawnTime(0)
, m_ownerID(0)
{
}

Projectile::~Projectile()
{
}
