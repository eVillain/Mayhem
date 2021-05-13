#include "Item.h"
#include "Core/Dispatcher.h"

Item::Item(const uint16_t entityID,
           const EntityType& type)
: Entity(entityID, type)
, m_ownerID(entityID)
, m_amount(0)
{
}

Item::~Item()
{
}
