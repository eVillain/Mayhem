#include "LootBox.h"
#include "SharedConstants.h"

LootBox::LootBox(const uint16_t entityID)
: Item(entityID, EntityType::Loot_Box)
{
}

const uint16_t LootBox::getInventoryAmount(const EntityType type) const
{
    auto it = std::find_if(m_inventory.begin(), m_inventory.end(), [type](const InventoryItem& inventoryItem){
        return type == inventoryItem.type;
    });
    
    if (it == m_inventory.end())
    {
        return 0;
    }
    
    return (*it).amount;
}

void LootBox::setInventoryAmount(const EntityType type, const uint16_t amount)
{
    auto it = std::find_if(m_inventory.begin(), m_inventory.end(), [type](const InventoryItem& inventoryItem){
        return type == inventoryItem.type;
    });
    
    if (it != m_inventory.end())
    {
        (*it).amount = amount;
        return;
    }
    
    m_inventory.push_back({type, amount});
}
