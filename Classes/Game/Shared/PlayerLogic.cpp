#include "PlayerLogic.h"
#include "EntityDataModel.h"
#include "SharedConstants.h"

void PlayerLogic::applyInput(const uint8_t playerID,
                             SnapshotData& snapshot,
                             std::shared_ptr<ClientInputMessage>& input,
                             float& lastActionTime,
                             const float currentTime)
{
    if (!input)
    {
        return;
    }
    
    auto playerIt = snapshot.playerData.find(playerID);
    if (playerIt == snapshot.playerData.end())
    {
        return;
    }
    PlayerState& player = playerIt->second;
    auto entityIt = snapshot.entityData.find(player.entityID);
    if (entityIt == snapshot.entityData.end())
    {
        return;
    }
    EntitySnapshot& entity = entityIt->second;
    
    if (input->changeWeapon)
    {
        uint8_t newSlot = input->slot - 1;
        newSlot = std::max(std::min(newSlot, (uint8_t)WeaponSlot::MELEE), (uint8_t)WeaponSlot::MAIN_1);
        if (player.activeWeaponSlot != newSlot)
        {
            player.activeWeaponSlot = newSlot;
        }
    }
    
    cocos2d::Vec2 direction = cocos2d::Vec2(input->directionX, input->directionY);
    cocos2d::Vec2 velocity = getMovementVelocityForInput(direction, input->run);
    bool flipX = player.flipX;
    const bool hasInputMovement = direction.length() > 0.f;
    if (hasInputMovement)
    {
        flipX = input->directionX < 0.f;
    }
    player.flipX = flipX;

    player.aimPointX = input->aimPointX;
    player.aimPointY = input->aimPointY;

    AnimationState animationState = AnimationState::Idle;
    
    if (checkForReload(snapshot, player, entity,  input, lastActionTime, currentTime))
    {
        animationState = AnimationState::Reload;
    }
    else
    {
        const cocos2d::Vec2 playerPosition = cocos2d::Vec2(entity.positionX, entity.positionY);
        const cocos2d::Vec2 aimPosition = cocos2d::Vec2(player.aimPointX, player.aimPointY);
        const InventoryItemState& weapon = player.weaponSlots.at(player.activeWeaponSlot);
        const StaticEntityData& weaponData = EntityDataModel::getStaticEntityData((EntityType)weapon.type);
        const uint16_t inventoryAmmo = getInventoryAmount((EntityType)weaponData.ammo.type, snapshot.inventory);
        uint32_t entityUnderCursorID = 0;
        bool entityUnderCursor = getEntityAtPoint(entityUnderCursorID, snapshot, aimPosition, player.entityID);
        
        if (input->reload &&
            weapon.amount == 0 &&
            inventoryAmmo > 0)
        {
            animationState = AnimationState::Reload;
        }
        else if (input->interact &&
                 aimPosition.distance(playerPosition) <= ITEM_GRAB_RADIUS &&
                 entityUnderCursor &&
                 EntityDataModel::isItemType((EntityType)snapshot.entityData.at(entityUnderCursorID).type))
        {
            animationState = AnimationState::Grab;
        }
        else if (velocity.length() > PLAYER_WALK_VEL)
        {
            animationState = AnimationState::Run;
        }
        else if (velocity.length() > PLAYER_IDLE_VEL_THRESHOLD)
        {
            animationState = AnimationState::Walk;
        }
    }

    player.animationState = animationState;
}

bool PlayerLogic::checkForReload(SnapshotData& snapshot,
                                 PlayerState& player,
                                 EntitySnapshot& entity,
                                 const std::shared_ptr<ClientInputMessage>& input,
                                 float& lastActionTime,
                                 const float currentTime)
{
    if (isReloading((AnimationState)player.animationState,
                    lastActionTime,
                    currentTime))
    {
        return true;
    }
    
    if (input->reload &&
        canReload(snapshot, player, lastActionTime, currentTime))
    {
        lastActionTime = currentTime;
        return true;
    }
    
    InventoryItemState& weapon = player.weaponSlots.at(player.activeWeaponSlot);
    if (weapon.type == EntityType::PlayerEntity)
    {
        return false;
    }
    
    const auto& itemData = EntityDataModel::getStaticEntityData((EntityType)weapon.type);
    const uint16_t inventoryAmmo = getInventoryAmount((EntityType)itemData.ammo.type, snapshot.inventory);
    const uint16_t ammo = weapon.amount;
    if (player.animationState == AnimationState::Reload)
    {
        const bool reloadFinished = lastActionTime + DEFAULT_RELOAD_TIME < currentTime;
        if (reloadFinished)
        {
            const uint16_t roomInMagazine = itemData.ammo.amount - ammo;
            const uint16_t ammoMove = std::min(roomInMagazine, inventoryAmmo);
            weapon.amount += ammoMove;
            
            setInventoryAmount((EntityType)itemData.ammo.type, inventoryAmmo - ammoMove, snapshot.inventory);
            lastActionTime = -1;
            return false;
        }
        return true;
    }
    return false;
}

bool PlayerLogic::isReloading(const AnimationState animationState,
                              const float lastActionTime,
                              const float currentTime)
{
    if (animationState != AnimationState::Reload)
    {
        return false;
    }
    return lastActionTime + DEFAULT_RELOAD_TIME > currentTime;
}

bool PlayerLogic::canReload(const SnapshotData& snapshot,
                            const PlayerState& player,
                            const float lastActionTime,
                            const float currentTime)
{
    if (isReloading((AnimationState)player.animationState,
                    lastActionTime,
                    currentTime))
    {
        return false;
    }
    
    const InventoryItemState& weapon = player.weaponSlots.at(player.activeWeaponSlot);
    const bool hasWeaponEquipped = weapon.type != EntityType::PlayerEntity;
    if (!hasWeaponEquipped)
    {
        return false;
    }
    
    const auto& itemData = EntityDataModel::getStaticEntityData((EntityType)weapon.type);
    const uint16_t ammo = weapon.amount;
    const bool hasFullMag = ammo == itemData.ammo.amount;
    if (hasFullMag)
    {
        return false;
    }
    
    const uint16_t inventoryAmmo = getInventoryAmount((EntityType)itemData.ammo.type, snapshot.inventory);
    const bool hasSpareAmmo = inventoryAmmo > 0;
    if (!hasSpareAmmo)
    {
        return false;
    }
    
    const float shotDelay = itemData.weapon.timeShot;
    const bool tooBusyToReload = (lastActionTime > 0.f &&
                                  lastActionTime + shotDelay > currentTime);
    return !tooBusyToReload;
}

bool PlayerLogic::getEntityAtPoint(uint32_t& outEntityID,
                                   const SnapshotData& snapshot,
                                   const cocos2d::Vec2& point,
                                   const uint32_t ignoreEntityID)
{
    for (const auto& entityPair : snapshot.entityData)
    {
        if (entityPair.first == ignoreEntityID)
        {
            continue;
        }
        const EntitySnapshot& entity = entityPair.second;
        const auto& collisionRects = EntityDataModel::getCollisionRects((EntityType)entity.type);
        for (const auto& baseRect : collisionRects)
        {
            const cocos2d::Vec2 position = cocos2d::Vec2(entity.positionX, entity.positionY);
            const cocos2d::Rect rect = cocos2d::Rect(baseRect.origin + position, baseRect.size);
            if (rect.containsPoint(point))
            {
                outEntityID = entityPair.first;
                return true;
            }
        }
    }
    
    return false;
}

uint16_t PlayerLogic::getInventoryAmount(const EntityType type,
                                         const std::vector<InventoryItemState>& inventory)
{
    auto it = std::find_if(inventory.begin(),
                           inventory.end(),
                           [type](const InventoryItemState& inventoryItem) {
        return type == inventoryItem.type;
    });
    
    if (it == inventory.end())
    {
        return 0;
    }
    
    return (*it).amount;
}

void PlayerLogic::setInventoryAmount(const EntityType type,
                                     const uint16_t amount,
                                     std::vector<InventoryItemState>& inventory)
{
    auto it = std::find_if(inventory.begin(),
                           inventory.end(),
                           [type](const InventoryItemState& inventoryItem) {
        return type == inventoryItem.type;
    });
    
    if (it != inventory.end())
    {
        (*it).amount = amount;
        return;
    }
    
    inventory.push_back({(uint8_t)type, amount});
}

cocos2d::Vec2 PlayerLogic::getMovementVelocityForInput(const cocos2d::Vec2& input,
                                                       const bool run)
{
    const bool hasInputMovement = input.lengthSquared() > 0.f;
    if (hasInputMovement)
    {
        const float moveSpeed = run ? PLAYER_RUN_VEL : PLAYER_WALK_VEL;
        return cocos2d::Vec2(input.x, -input.y).getNormalized() * moveSpeed;
    }
    return cocos2d::Vec2::ZERO;
}
