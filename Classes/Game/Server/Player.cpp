#include "Player.h"
#include "SharedConstants.h"
#include "EntityDataModel.h"

Player::Player(const uint16_t entityID)
: Entity(entityID, EntityType::PlayerEntity)
, m_lastActionTime(0.f)
, m_health(PLAYER_DEFAULT_HEALTH)
, m_animationState(Idle)
, m_flipX(false)
, m_activeSlot(WeaponSlot::MELEE)
, m_weaponSlots(5)
{
    m_armor.push_back({ArmorType::Armor_Type_Helmet, 0, 0});
    m_armor.push_back({ArmorType::Armor_Type_Vest, 0, 0});
}

const uint16_t Player::getInventoryAmount(const EntityType type) const
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

void Player::setInventoryAmount(const EntityType type, const uint16_t amount)
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

const bool Player::hasWeaponEquipped() const
{
    const InventoryItem& weapon = m_weaponSlots.at(m_activeSlot);
    return weapon.type != EntityType::PlayerEntity;
}

const bool Player::canReload(const float currentTime) const
{
    if (isReloading(currentTime))
    {
        return false;
    }
    
    const InventoryItem& weapon = m_weaponSlots.at(m_activeSlot);
    const bool hasWeaponEquipped = weapon.type != EntityType::PlayerEntity;
    if (!hasWeaponEquipped)
    {
        return false;
    }
    
    const auto& itemData = EntityDataModel::getStaticEntityData(weapon.type);
    const uint16_t ammo = weapon.amount;
    const bool hasFullMag = ammo == itemData.ammo.amount;
    if (hasFullMag)
    {
        return false;
    }
    
    const uint16_t inventoryAmmo = getInventoryAmount((EntityType)itemData.ammo.type);
    const bool hasSpareAmmo = inventoryAmmo > 0;
    if (!hasSpareAmmo)
    {
        return false;
    }
    
    const float shotDelay = itemData.weapon.timeShot;
    const bool tooBusyToReload = (m_lastActionTime > 0.f &&
                                  m_lastActionTime + shotDelay > currentTime);
    return !tooBusyToReload;
}

const bool Player::isReloading(const float currentTime) const
{
    if (m_animationState != AnimationState::Reload)
    {
        return false;
    }
    return m_lastActionTime + DEFAULT_RELOAD_TIME > currentTime;
}
