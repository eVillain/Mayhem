#ifndef ShowInventoryEvent_h
#define ShowInventoryEvent_h

#include "Core/Event.h"
#include <memory>
#include <vector>

class Player;
class Entity;

class ShowInventoryEvent : public Event
{
public:
    ShowInventoryEvent(const std::shared_ptr<Player>& player,
                       const std::vector<std::shared_ptr<Entity>>& nearEntities);
    
    virtual DescriptorType type() const { return descriptor; }

    static constexpr DescriptorType descriptor = "ShowInventory";
    
    const std::shared_ptr<Player>& player;
    const std::vector<std::shared_ptr<Entity>>& nearEntities;
};

#endif /* ShowInventoryEvent_h */
