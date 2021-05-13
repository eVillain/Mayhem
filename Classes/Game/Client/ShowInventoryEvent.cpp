#include "ShowInventoryEvent.h"

constexpr ShowInventoryEvent::DescriptorType ShowInventoryEvent::descriptor;

ShowInventoryEvent::ShowInventoryEvent(const std::shared_ptr<Player>& player,
                                       const std::vector<std::shared_ptr<Entity>>& nearEntities)
: player(player)
, nearEntities(nearEntities)
{
}
