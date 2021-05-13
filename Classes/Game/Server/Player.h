#ifndef Player_h
#define Player_h

#include "Entity.h"
#include <memory>

class Player : public Entity
{
public:
    Player(const uint16_t entityID);
    
    const uint8_t getPlayerID() const { return m_playerID; }
    void setPlayerID(const uint8_t playerID) { m_playerID = playerID; }

    const cocos2d::Vec2& getAimPosition() const { return m_aimPosition; }
    float getLastActionTime() const { return m_lastActionTime; }
    float getHealth() const { return m_health; }
        
    void setAimPosition(const cocos2d::Vec2& aimPosition) { m_aimPosition = aimPosition; }
    void setLastActionTime(const float time) { m_lastActionTime = time; }
    void setHealth(const float health) { m_health = health; }
    
    void setAnimationState(const AnimationState state) { m_animationState = state; }
    const AnimationState getAnimationState() const { return m_animationState; }
    void setFlipX(const bool flipX) { m_flipX = flipX; }
    bool getFlipX() const { return m_flipX; }
    
    // ---- Required data for current player only ---- //
    void setActiveSlot(const WeaponSlot slot) { m_activeSlot = slot; }
    const WeaponSlot getActiveSlot() const { return m_activeSlot; }
        
    std::vector<InventoryItem>& getWeaponSlots() { return m_weaponSlots; }
    std::vector<InventoryItem>& getInventory() { return m_inventory; }
    ArmorData& getArmor(const ArmorType type) { return m_armor.at(type); }

    const uint16_t getInventoryAmount(const EntityType type) const;
    void setInventoryAmount(const EntityType type, const uint16_t amount);

    const bool hasWeaponEquipped() const;
    const bool canReload(const float currentTime) const;
    const bool isReloading(const float currentTime) const;

private:
    uint8_t m_playerID;
    cocos2d::Vec2 m_aimPosition;
    float m_lastActionTime;
    float m_health;

    AnimationState m_animationState;
    bool m_flipX;
    WeaponSlot m_activeSlot;
    
    std::vector<InventoryItem> m_weaponSlots;
    std::vector<InventoryItem> m_inventory;
    std::vector<ArmorData> m_armor;
};

#endif /* Player_h */
