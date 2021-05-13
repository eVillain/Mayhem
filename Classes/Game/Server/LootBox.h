#ifndef LootBox_h
#define LootBox_h

#include "Item.h"
#include <memory>

class LootBox : public Item
{
public:
    LootBox(const uint16_t entityID);

    std::vector<InventoryItem>& getInventory() { return m_inventory; }
    
    const uint16_t getInventoryAmount(const EntityType type) const;
    void setInventoryAmount(const EntityType type, const uint16_t amount);

private:
    std::vector<InventoryItem> m_inventory;
};

#endif /* LootBox_h */
