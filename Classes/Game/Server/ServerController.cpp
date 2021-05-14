#include "ServerController.h"

#include "BaseAI.h"
#include "Core/Dispatcher.h"
#include "EntitiesController.h"
#include "EntitiesModel.h"
#include "Entity.h"
#include "EntityDataModel.h"
#include "FrameCache.h"
#include "Game/Client/InputModel.h"
#include "GameController.h"
#include "GameModel.h"
#include "InitMainMenuCommand.h"
#include "InputCache.h"
#include "InputController.h"
#include "Item.h"
#include "LevelModel.h"
#include "LootBox.h"
#include "NetworkController.h"
#include "Player.h"
#include "Projectile.h"
#include "ReliabilitySystem.h"
#include "SharedConstants.h"
#include "WeaponConstants.h"

#include <algorithm>
#include <random>

ServerController::ServerController(std::shared_ptr<GameController> gameController,
                                   std::shared_ptr<LevelModel> levelModel,
                                   std::shared_ptr<GameModel> gameModel,
                                   std::shared_ptr<INetworkController> networkController,
                                   std::shared_ptr<FrameCache> frameCache,
                                   std::shared_ptr<InputCache> inputCache)
: m_gameController(gameController)
, m_levelModel(levelModel)
, m_gameModel(gameModel)
, m_networkController(networkController)
, m_inputCache(inputCache)
, m_frameCache(frameCache)
, m_maxPingThreshold(0.6f) // TODO: Set a reasonable threshold value
, m_sendDeltaUpdates(false)
{    
    m_networkController->addMessageCallback(MessageTypes::MESSAGE_TYPE_CLIENT_STATE_UPDATE,
                                            std::bind(&ServerController::onClientStateMessageReceived, this,
                                                      std::placeholders::_1, std::placeholders::_2));
    m_networkController->addMessageCallback(MessageTypes::MESSAGE_TYPE_CLIENT_INPUT,
                                            std::bind(&ServerController::onInputMessageReceived, this,
                                                      std::placeholders::_1, std::placeholders::_2));
    m_networkController->setNodeConnectedCallback(std::bind(&ServerController::onNodeConnected, this, std::placeholders::_1));
    m_networkController->setNodeDisconnectedCallback(std::bind(&ServerController::onNodeDisconnected, this, std::placeholders::_1));
    m_frameCache->setMaxRollbackFrames(((m_maxPingThreshold + DEFAULT_INTERPOLATION_LATENCY) / m_gameModel->getFrameTime()) + 10);
}

ServerController::~ServerController()
{
}

void ServerController::update(float deltaTime)
{
    const float deltaAccumulator = m_gameModel->getDeltaAccumulator() + deltaTime;
    m_gameModel->setDeltaAccumulator(deltaAccumulator);

    m_networkController->receiveMessages();
    m_networkController->update(deltaTime);
    
    bool sendUpdate = false;
    
    const float frameTime = m_gameModel->getFrameTime();
    while (m_gameModel->getDeltaAccumulator() >= frameTime)
    {
        m_gameModel->setDeltaAccumulator(m_gameModel->getDeltaAccumulator() - frameTime);
        m_gameModel->setCurrentTime(m_gameModel->getCurrentTime() + frameTime);

        applyAI();
        performGameUpdate(frameTime);
        
        m_gameModel->setCurrentTick(m_gameModel->getCurrentTick() + 1);
        
        sendUpdate = true;
    }
    
    if (sendUpdate)
    {
        const auto postTickState = m_gameController->getEntitiesModel()->getSnapshot();
        const auto players = m_gameController->getEntitiesModel()->getPlayers();
        
        SnapshotData snapshot;
        snapshot.serverTick = m_gameModel->getCurrentTick();
        snapshot.entityCount = (uint32_t)postTickState.size();
        snapshot.entityData = postTickState;
        snapshot.playerCount = players.size();
        snapshot.hitData = m_frameHitData;

        for (auto player : players)
        {
            snapshot.playerData[player.first].entityID = player.second->getEntityID();
            snapshot.playerData[player.first].animationState = player.second->getAnimationState();
            snapshot.playerData[player.first].aimPointX = player.second->getAimPosition().x;
            snapshot.playerData[player.first].aimPointY = player.second->getAimPosition().y;
            snapshot.playerData[player.first].health = player.second->getHealth();
            snapshot.playerData[player.first].flipX = player.second->getFlipX();
            snapshot.playerData[player.first].activeWeaponSlot = player.second->getActiveSlot();
            const auto& weaponSlots = player.second->getWeaponSlots();
            for (size_t i = 0; i < 5; i++)
            {
                snapshot.playerData[player.first].weaponSlots.push_back({(uint8_t)weaponSlots.at(i).type, weaponSlots.at(i).amount});
            }
        }
        
        for (const auto& clientState : m_clientStates)
        {
            const uint8_t playerID = clientState.first;
            if (clientState.second == ClientPlayerState::DISCONNECTED)
            {
                continue;
            }
            else if (clientState.second == ClientPlayerState::JOINING)
            {
                std::shared_ptr<ServerLoadLevelMessage> loadLevelMessage = std::make_shared<ServerLoadLevelMessage>();
                loadLevelMessage->tickRate = m_gameModel->getTickRate();
                loadLevelMessage->modeType = m_gameController->getGameMode()->getType();
                loadLevelMessage->maxPlayers = m_gameController->getGameMode()->getMaxPlayers();
                loadLevelMessage->playersPerTeam = m_gameController->getGameMode()->getPlayersPerTeam();
                loadLevelMessage->level = m_levelModel->getLevel();
                std::shared_ptr<Net::Message> message = loadLevelMessage;
                m_networkController->sendMessage(playerID, message);
                continue;
            }
            
            snapshot.lastReceivedInput = m_inputCache->getLastReceivedSequence(playerID);
            const auto& sendingPlayer = players.find(playerID);
            if (sendingPlayer != players.end())
            {
                auto& inventory = sendingPlayer->second->getInventory();
                for (const auto& inventoryItem : inventory)
                {
                    snapshot.inventory.push_back({(uint8_t)inventoryItem.type, inventoryItem.amount});
                }
            }
            
            if (m_sendDeltaUpdates)
            {
                auto& playerSnapshots = m_clientSnapshots.at(playerID);
                playerSnapshots.push_back(snapshot);

                const uint32_t lastReceivedSnapshotTick = m_inputCache->getLastReceivedSnapshot(playerID);
                auto snapshotIt = std::find_if(playerSnapshots.begin(),
                                               playerSnapshots.end(),
                                               [lastReceivedSnapshotTick](const SnapshotData& data){
                    return data.serverTick == lastReceivedSnapshotTick;
                });
                if (snapshotIt != playerSnapshots.end())
                {
                    const auto& lastReceivedState = *snapshotIt;
                    std::shared_ptr<ServerSnapshotDiffMessage> deltaMessage = std::make_shared<ServerSnapshotDiffMessage>([lastReceivedState](const uint32_t){ return lastReceivedState; });
                    deltaMessage->data = snapshot;
                    deltaMessage->previousServerTick = lastReceivedSnapshotTick;
                    std::shared_ptr<Net::Message> message = std::dynamic_pointer_cast<Net::Message>(deltaMessage);
                    m_networkController->sendMessage(playerID, message);
                }
                
                // Erase up to last received
                playerSnapshots.erase(std::remove_if(playerSnapshots.begin(),
                                                     playerSnapshots.end(),
                                                     [lastReceivedSnapshotTick](const SnapshotData& data)
                                                     {
                                                        return data.serverTick < lastReceivedSnapshotTick;
                                                    }),
                                      playerSnapshots.end());
            }
            else
            {
                std::shared_ptr<ServerSnapshotMessage> snapshotMessage = std::make_shared<ServerSnapshotMessage>();
                snapshotMessage->data = snapshot;

                std::shared_ptr<Net::Message> message = std::dynamic_pointer_cast<Net::Message>(snapshotMessage);
                m_networkController->sendMessage(playerID, message);
            }
//            CCLOG("ServerController::update sent snapshot to player %i", sendingPlayer.first);
        }
        
        m_networkController->sendMessages();
        m_frameHitData.clear();
    }
}

void ServerController::stop()
{
    m_inputCache->clear();
    m_networkController->removeMessageCallback(MessageTypes::MESSAGE_TYPE_CLIENT_STATE_UPDATE);
    m_networkController->removeMessageCallback(MessageTypes::MESSAGE_TYPE_CLIENT_INPUT);
    m_networkController->setNodeConnectedCallback(nullptr);
    m_networkController->setNodeDisconnectedCallback(nullptr);
    m_networkController->getTransport()->setDisconnectedCallback(nullptr);
    InitMainMenuCommand initMainMenu;
    initMainMenu.run();
}

const std::string ServerController::getDebugInfo() const
{
    auto& inputs = m_inputCache->getInputData();
    
    const std::string input0 = inputs[0].size() ? std::to_string(inputs[0].back().clientInput->inputSequence) : "x";
    const std::string input1 = inputs[1].size() ? std::to_string(inputs[1].back().clientInput->inputSequence) : "x";
    return "Server Tick:" + std::to_string(m_gameModel->getCurrentTick()) + "Inputs P0: " + std::to_string(inputs[0].size()) + "/" + input0 +
                        " P1: " + std::to_string(inputs[1].size()) + "/" + input1;
}

void ServerController::performGameUpdate(const float deltaTime)
{
    // Check for unprocessed inputs
    const auto inputData = m_inputCache->getCombinedInputs(); // map of inputs
    const auto& players = m_gameController->getEntitiesModel()->getPlayers();
    std::map<uint8_t, std::shared_ptr<ClientInputMessage>> inputs;
    for (auto pair : inputData) // first == playerID, second == inputs
    {
        const uint8_t playerID = pair.first;
        auto playerIt = players.find(playerID);
        if (playerIt == players.end() ||
            playerIt->second->getHealth() <= 0.f)
        {
            continue; // Player doesn't exist or is dead
        }
        
        inputs[playerID] = pair.second.clientInput;
        
        bool rollbackRequired = (inputs[playerID]->shoot && wasWeaponFired(pair.first)) ||
                                inputs[playerID]->interact;
        
        if (rollbackRequired) // Roll back world to client state
        {
            rollbackForPlayer(playerID,
                              inputs[playerID]->lastReceivedSnapshot);
        }
        
        if (inputs[pair.first]->shoot)
        {
            checkForShots(pair.first, inputs[pair.first]);
        }
        if (inputs[pair.first]->interact)
        {
            checkForInteraction(pair.first, inputs[pair.first]);
        }
        
        if (rollbackRequired) // Reset world to server state
        {
            m_gameController->getEntitiesModel()->setSnapshot(m_preRollbackState);
        }
    }
    
    // Current inputs have been processed, clear them for the next frame
    m_inputCache->getInputData().clear();
                
    m_gameController->applyInputs(inputs);

    std::map<uint32_t, EntitySnapshot> worldState = m_gameController->getEntitiesModel()->getSnapshot();
    
    MovementIntegrator::integratePositions(m_gameModel->getFrameTime(),
                                           worldState,
                                           m_levelModel->getStaticRects(),
                                           std::bind(&ServerController::onEntityCollision, this, std::placeholders::_1));
    m_gameController->getEntitiesModel()->setSnapshot(worldState);
    
    m_gameController->tick(m_gameModel->getFrameTime());

    // Remove all entities which have expired
    removeExpiredEntities();
        
    // Save state after simulating
    m_frameCache->takeFrameSnapshot(worldState);
}

void ServerController::checkForShots(uint8_t playerID, const std::shared_ptr<ClientInputMessage>& input)
{
    auto player = m_gameController->getEntitiesModel()->getPlayer(playerID);
    InventoryItem& weapon = player->getWeaponSlots().at(player->getActiveSlot());
    const EntityType heldItemType = weapon.type;
    if (heldItemType == EntityType::PlayerEntity)
    {
        return;
    }
    
    auto itemData = EntityDataModel::getStaticEntityData(heldItemType);
    float shotDelay = itemData.weapon.timeShot;
    const size_t ammo = weapon.amount;
    const bool canFire = (player->getLastActionTime() < 0.f ||
                          player->getLastActionTime() + shotDelay < m_gameModel->getCurrentTime());
    if (ammo > 0 && canFire)
    {
        // Fire weapon!
        weapon.amount = weapon.amount - 1;
        player->setLastActionTime(m_gameModel->getCurrentTime());

        const cocos2d::Vec2 aimPoint = cocos2d::Vec2(input->aimPointX, input->aimPointY);
        const WeaponConstants::WeaponStateData data = WeaponConstants::getWeaponData(player->getPosition(),
                                                                                     aimPoint,
                                                                                     itemData.weapon.holdOffset,
                                                                                     itemData.weapon.projectileOffset,
                                                                                     player->getFlipX());
        if (itemData.weapon.damageType == DamageType::Damage_Type_Hitscan)
        {
            const int numShots = itemData.weapon.type == WeaponType::Weapon_Type_Shotgun ? 8 : 1;

            const float RAY_LENGTH = itemData.weapon.type == WeaponType::Weapon_Type_Shotgun ? 128 : 512.f;

            const cocos2d::Vec2 shotEnd = data.projectilePosition + (aimPoint - data.projectilePosition).getNormalized() * RAY_LENGTH;
            const cocos2d::Vec2 offset = (data.projectilePosition - shotEnd).getNormalized().getPerp() * 2.f;

            for (int shot = 0; shot < numShots; shot++)
            {
                const int shotOffsetRanged = shot - (numShots/2);
                const cocos2d::Vec2 shotOffset = numShots == 1 ? cocos2d::Vec2:: ZERO : offset * shotOffsetRanged;
                const RayElement raycastResult = fireHitScanWeapon(data.projectilePosition,
                                                                    shotEnd + shotOffset,
                                                                    player->getEntityID(),
                                                                    heldItemType,
                                                                    itemData.weapon.damageAmount);
            }
        }
        else if (itemData.weapon.damageType == DamageType::Damage_Type_Projectile)
        {
            // Spawn projectile
            const EntityType projectileType = itemData.weapon.projectileType;
            const uint16_t entityID = m_gameController->getEntitiesModel()->getNextEntityID();
            m_gameController->getEntitiesModel()->incrementNextEntityID();
            cocos2d::Vec2 velocity = aimPoint - data.projectilePosition;
            if (projectileType == EntityType::Projectile_Rocket)
            {
                velocity.normalize();
                velocity *= 300.f;
            }
            else if (projectileType == EntityType::Projectile_Bullet)
            {
                velocity.normalize();
                velocity *= 600.f;
            }
            auto projectile = m_gameController->getEntitiesController()->createProjectile(entityID,
                                                                                          projectileType,
                                                                                          data.projectilePosition,
                                                                                          velocity,
                                                                                          data.projectileAngle);
            projectile->setOwnerID(playerID);
            projectile->setSpawnTime(m_gameModel->getCurrentTime());
            if (weapon.amount == 0) // Ran out of throwables of this type
            {
                weapon.type = EntityType::PlayerEntity;
            }
        }
    }
}

const RayElement ServerController::fireHitScanWeapon(const cocos2d::Vec2& shotStart,
                                                     const cocos2d::Vec2& shotEnd,
                                                     const uint16_t entityID,
                                                     const EntityType weaponType,
                                                     const int damage)
{
    RayElement raycastResult = RaycastUtil::rayCast(shotStart,
                                                    shotEnd,
                                                    entityID,
                                                    m_gameController->getEntitiesModel()->getEntities(),
                                                    m_gameController->getLevelModel()->getStaticRects());
    if (raycastResult.hitEntityID != 0)
    {
        auto hitEntity = m_gameController->getEntitiesModel()->getEntities().at(raycastResult.hitEntityID);
        if (hitEntity->getEntityType() == PlayerEntity)
        {
            std::shared_ptr<Player> hitPlayer = std::dynamic_pointer_cast<Player>(hitEntity);
            if (!hitPlayer->getIsRemoved())
            {
                applyDamage(hitPlayer, damage, raycastResult.shotEndPoint, entityID, weaponType, raycastResult.hitShapeIndex);
                return raycastResult;
            }
        }
    }

    return raycastResult;
}

void ServerController::checkForInteraction(uint8_t playerID, const std::shared_ptr<ClientInputMessage>& input)
{
    auto player = m_gameController->getEntitiesModel()->getPlayer(playerID);
    const cocos2d::Vec2 playerPosition = player->getPosition();
    if (input->pickUpType > 0)
    {
        const auto& entities = m_gameController->getEntitiesModel()->getEntities();
        const auto& it = entities.find(input->pickUpID);
        if (it != entities.end())
        {
            if (std::shared_ptr<Item> item = std::dynamic_pointer_cast<Item>(it->second))
            {
                onItemPickedUp(player.get(), item.get());
            }
        }
    }
    else
    {
        const cocos2d::Vec2 aimPosition = cocos2d::Vec2(input->aimPointX, input->aimPointY);
        if (aimPosition.distance(playerPosition) > ITEM_GRAB_RADIUS)
        {
            return;
        }
        
        std::shared_ptr<Entity> entity = m_gameController->getEntityAtPoint(aimPosition, player->getEntityID());
        if (!entity)
        {
            return;
        }
        
        if (std::shared_ptr<Item> item = std::dynamic_pointer_cast<Item>(entity))
        {
            if (!item->getIsRemoved()) // If item was already destroyed/picked up this player is simply denied
            {
                onItemPickedUp(player.get(), item.get());
            }
            return;
        }
    }
    
    // TODO: Other forms of interaction might follow here, ie. doors etc
}

void ServerController::removeExpiredEntities()
{
    const auto& entities = m_gameController->getEntitiesModel()->getEntities();
    for (auto entityPair : entities)
    {
        const auto projectile = std::dynamic_pointer_cast<Projectile>(entityPair.second);
        if (!projectile)
        {
            continue;
        }
        
        auto itemData = EntityDataModel::getStaticEntityData(projectile->getEntityType());
        const float expireTime = projectile->getSpawnTime() + itemData.weapon.timeReload;
        if (expireTime > m_gameModel->getCurrentTime())
        {
            continue;
        }
        
        onProjectileDestroyed(projectile);
    }
}

void ServerController::onEntityCollision(const CollisionData& collisionData)
{
    const auto& entities = m_gameController->getEntitiesModel()->getEntities();
    auto entity = entities.find(collisionData.entityID);
    if (entity == entities.end())
    {
        return;
    }
    const auto projectile = std::dynamic_pointer_cast<Projectile>(entity->second);
    if (!projectile)
    {
        return;
    }
    
    std::shared_ptr<Entity> colliderEntity = nullptr;
    if (!collisionData.isStaticCollider)
    {
        auto colliderEntityIt = entities.find(collisionData.colliderID);
        if (colliderEntityIt != entities.end())
        {
            colliderEntity = colliderEntityIt->second;
        }
    }
    
    onProjectileHit(colliderEntity, projectile, collisionData.hitShapeIndex);
}

bool ServerController::onItemPickedUp(Player* player, Item* item)
{
    // Determine item type first
    const EntityType type = item->getEntityType();
    const auto& itemData = EntityDataModel::getStaticEntityData(type);
    const bool isWeapon = (type > EntityType::Item_First_Placeholder &&
                           type <= EntityType::Item_Nade_Smoke);
    const bool isArmor = (type >= EntityType::Item_Helmet_Level2 &&
                           type <= EntityType::Item_Helmet_Level3);
    bool storeInInventory = !isWeapon && !isArmor;
    if (isWeapon)
    {
        const bool isPrimary = (itemData.weapon.type == WeaponType::Weapon_Type_Sniper ||
                                itemData.weapon.type == WeaponType::Weapon_Type_AR ||
                                itemData.weapon.type == WeaponType::Weapon_Type_Launcher ||
                                itemData.weapon.type == WeaponType::Weapon_Type_Shotgun ||
                                itemData.weapon.type == WeaponType::Weapon_Type_SMG);
        const bool isSecondary = (itemData.weapon.type == WeaponType::Weapon_Type_Pistol);
        const bool isThrowable = (itemData.weapon.type == WeaponType::Weapon_Type_Throwable);
        
        const WeaponSlot activeSlot = player->getActiveSlot();
        auto& weaponSlots = player->getWeaponSlots();
        WeaponSlot targetSlot;
        
        if (isPrimary)
        {
            if (weaponSlots.at(WeaponSlot::MAIN_1).type != EntityType::PlayerEntity)
            {
                if (weaponSlots.at(WeaponSlot::MAIN_2).type != EntityType::PlayerEntity)
                {
                    targetSlot = activeSlot;
                }
                else
                {
                    targetSlot = WeaponSlot::MAIN_2;
                }
            }
            else
            {
                targetSlot = WeaponSlot::MAIN_1;
            }
        }
        else if (isSecondary)
        {
            targetSlot = WeaponSlot::SECONDARY;
        }
        else if (isThrowable)
        {
            targetSlot = WeaponSlot::THROWABLE;
        }
        else
        {
            assert(false); // WTF kind of entity type is this then? New weapon type?
            return false;
        }

        if (weaponSlots.at(targetSlot).type != EntityType::PlayerEntity)
        {
            if (isThrowable)
            {
                storeInInventory = true;
            }
            else
            {
                // Replace weapon in slot, previously held weapon drops to the floor
                auto entitiesModel = m_gameController->getEntitiesModel();
                auto droppedItem = m_gameController->getEntitiesController()->createItem(entitiesModel->getNextEntityID(),
                                                                                         weaponSlots.at(targetSlot).type,
                                                                                         player->getPosition() + cocos2d::Vec2(0, 8),
                                                                                         0.f);
                entitiesModel->incrementNextEntityID();
                droppedItem->setAmount(weaponSlots.at(targetSlot).amount);
            }
        }
        if (targetSlot != activeSlot)
        {
            player->setActiveSlot(targetSlot);
        }
        
        weaponSlots.at(targetSlot).type = type;
        weaponSlots.at(targetSlot).amount = item->getAmount();
    }
    
    if (isArmor)
    {
        auto& armor = player->getArmor(itemData.armor.type);
        if (armor.level)
        {
            // Drop previous armor on floor
            const EntityType previousArmorType = getArmorType(armor.type, armor.level);
            auto entitiesModel = m_gameController->getEntitiesModel();
            auto droppedItem = m_gameController->getEntitiesController()->createItem(entitiesModel->getNextEntityID(),
                                                                                     previousArmorType,
                                                                                     player->getPosition() + cocos2d::Vec2(0, 8),
                                                                                     0.f);
            entitiesModel->incrementNextEntityID();
        }
        armor.level = itemData.armor.level;
        armor.amount = item->getAmount();
    }

    if (storeInInventory)
    {
        auto& inventory = player->getInventory();
        auto it = std::find_if(inventory.begin(), inventory.end(), [type](const InventoryItem& inventoryItem){
            return type == inventoryItem.type;
        });
        if (it != inventory.end())
        {
            (*it).amount += item->getAmount();
        }
        else
        {
            inventory.push_back({type, item->getAmount()});
        }
    }
    
    player->setLastActionTime(-1.f);
    item->setIsRemoved(true);
    
    m_gameController->getEntitiesController()->destroyEntity(item->getEntityID());
    
    return false;
}

bool ServerController::onProjectileHit(const std::shared_ptr<Entity>& entity,
                                       const std::shared_ptr<Projectile>& projectile,
                                       const size_t hitShapeIndex)
{
    if (projectile->getEntityType() == Projectile_Grenade ||
        projectile->getEntityType() == Projectile_Smoke)
    {
        return false;
    }
    
    auto itemData = EntityDataModel::getStaticEntityData(projectile->getEntityType());
    if (itemData.weapon.damageType == DamageType::Damage_Type_Projectile)
    {
        if (entity && entity->getEntityType() == EntityType::PlayerEntity)
        {
            const auto& player = m_gameController->getEntitiesModel()->getPlayerByEntityID(entity->getEntityID());
            if (player && !player->getIsRemoved())
            {
                const auto& ownerPlayer = m_gameController->getEntitiesModel()->getPlayer(projectile->getOwnerID());
                const uint16_t ownerEntityID = ownerPlayer ? ownerPlayer->getEntityID() : 0;

                applyDamage(player, itemData.weapon.damageAmount, projectile->getPosition(), ownerEntityID, projectile->getEntityType(), hitShapeIndex);
            }
        }

        projectile->setIsRemoved(true);
        m_gameController->getEntitiesController()->destroyEntity(projectile->getEntityID());
    }
    else if (itemData.weapon.damageType == DamageType::Damage_Type_Explosion)
    {
        onProjectileDestroyed(projectile);
    }
    return true;
}

void ServerController::onProjectileDestroyed(const std::shared_ptr<Projectile>& projectile)
{
    const auto& itemData = EntityDataModel::getStaticEntityData(projectile->getEntityType());
    const auto& entities = m_gameController->getEntitiesModel()->getEntities();

    const auto ownerPlayer = m_gameController->getEntitiesModel()->getPlayer(projectile->getOwnerID());
    const uint16_t ownerEntityID = ownerPlayer ? ownerPlayer->getEntityID() : 0;
    if (itemData.weapon.damageType == DamageType::Damage_Type_Explosion)
    {
        for (auto explosionPair : entities)
        {
            if (explosionPair.second->getEntityType() != EntityType::PlayerEntity)
            {
                continue;
            }
            auto player = std::dynamic_pointer_cast<Player>(explosionPair.second);
            
            const cocos2d::Vec2 distVec = projectile->getPosition() - explosionPair.second->getPosition();
            const float dist = distVec.length();
            if (dist < EXPLOSION_RADIUS &&
                !player->getIsRemoved())
            {
                // TODO: Check for objects blocking explosion path by raycast
                const float damageMultilpier = (dist <= 16.f) ? 1.f : (dist / EXPLOSION_RADIUS);
                const float damage = itemData.weapon.damageAmount * damageMultilpier;
                applyDamage(player, damage, projectile->getPosition(), ownerEntityID, EntityType::ExplosionEntity, 0);
            }
        }
    }
    
    projectile->setIsRemoved(true);
    m_gameController->getEntitiesController()->destroyEntity(projectile->getEntityID());
}

void ServerController::applyDamage(const std::shared_ptr<Player>& player,
                                   const float damage,
                                   const cocos2d::Vec2 position,
                                   const uint16_t damagerID,
                                   const EntityType damageType,
                                   const size_t hitShapeIndex)
{
    const bool headShot = hitShapeIndex == 1;
    const float totalDamage = damage * (headShot ? DEFAULT_HEADSHOT_DAMAGE_MULTIPLIER : 1.f);
    const float newHealth = std::max(player->getHealth() - totalDamage, 0.f);
    
    FrameHitData hit;
    hit.hitterEntityID = damagerID;
    hit.hitEntityID = player->getEntityID();
    hit.damage = totalDamage;
    hit.hitPosX = position.x;
    hit.hitPosY = position.y;
    hit.headShot = headShot;
    m_frameHitData.push_back(hit);
    
    player->setHealth(newHealth);
    if (newHealth == 0.f)
    {
        auto entitiesModel = m_gameController->getEntitiesModel();
        player->setIsRemoved(true);

        // Spawn loot box at player position containing player inventory
        const uint16_t lootBoxEntityID = m_gameController->getEntitiesModel()->getNextEntityID();
        entitiesModel->incrementNextEntityID();
        std::shared_ptr<Item> lootBoxItem = m_gameController->getEntitiesController()->createItem(lootBoxEntityID,
                                                                                                  EntityType::Loot_Box,
                                                                                                  player->getPosition(),
                                                                                                  0.f);
        lootBoxItem->setOwnerID(player->getPlayerID());
        std::shared_ptr<LootBox> lootBox = std::dynamic_pointer_cast<LootBox>(lootBoxItem);
        const auto& inventory = player->getInventory();
        for (const auto& item : inventory)
        {
            lootBox->setInventoryAmount(item.type, item.amount);
        }

        // Store non-held weapons in loot box
        for (int i = 0; i < 5; i++)
        {
            const auto& weapon = player->getWeaponSlots().at(i);
            if (weapon.type == EntityType::PlayerEntity)
            {
                continue;
            }
            
            if (i != player->getActiveSlot())
            {
                lootBox->setInventoryAmount(weapon.type, weapon.amount);
            }
            else
            {
                // Drop held weapon on ground
                auto droppedItem = m_gameController->getEntitiesController()->createItem(entitiesModel->getNextEntityID(),
                                                                                         weapon.type,
                                                                                         player->getPosition() + cocos2d::Vec2(0, 8),
                                                                                         0.f);
                entitiesModel->incrementNextEntityID();
                droppedItem->setAmount(weapon.amount);
            }
        }
        
        const uint8_t playerID = player->getPlayerID();
        const uint16_t entityID = player->getEntityID();
        // We notify the GameMode before removing any data
        m_gameController->getGameMode()->onPlayerDied(playerID);
        entitiesModel->removePlayer(playerID);
        m_gameController->getEntitiesController()->destroyEntity(entityID);
        
        std::shared_ptr<ServerPlayerDeathMessage> deathMessage = std::make_shared<ServerPlayerDeathMessage>();
        deathMessage->deadPlayerID = playerID;
        deathMessage->killerEntityID = damagerID;
        deathMessage->killerType = damageType;
        deathMessage->headshot = headShot;
        std::shared_ptr<Net::Message> message = std::dynamic_pointer_cast<Net::Message>(deathMessage);
        m_networkController->sendMessage(playerID, message);
        
        const auto& killerPlayer = m_gameController->getEntitiesModel()->getPlayerByEntityID(damagerID);
        if (killerPlayer)
        {
            m_networkController->sendMessage(killerPlayer->getPlayerID(), message);
        }
    }
}

bool ServerController::wasWeaponFired(uint8_t playerID)
{
    auto player = m_gameController->getEntitiesModel()->getPlayer(playerID);
    const EntityType heldItemType = player->getWeaponSlots().at(player->getActiveSlot()).type;
    if (!heldItemType)
    {
        return false;
    }
    
    auto itemData = EntityDataModel::getStaticEntityData(heldItemType);
    float shotDelay = itemData.weapon.timeShot;
    
    return (player->getLastActionTime() < 0.f ||
            player->getLastActionTime() + shotDelay < m_gameModel->getCurrentTime());
}

void ServerController::rollbackForPlayer(const uint8_t playerID, const uint32_t lastReceivedSnapshot)
{
    // Save current state to be applied after shot/interaction has been processed
    m_preRollbackState = m_gameController->getEntitiesModel()->getSnapshot();
    
    // Roll back network latency + interpolation rate for everyone else before ticking
    const float networkLatency = m_networkController->GetRoundTripTime(playerID) * 0.5f; // 0.5 because only one-way latency counts here
    const uint32_t rollbackLatency = (networkLatency + DEFAULT_INTERPOLATION_LATENCY) / m_gameModel->getFrameTime();
    if (m_frameCache->getFrameCount() < rollbackLatency)
    {
        printf("ServerController::rollbackForPlayer %i failed because not enough frames in cache\n", playerID);
        return;
    }
//    printf("ServerController::rollbackForPlayer %i is %i frames (%fms)\n", playerID, rollbackLatency, networkLatency + DEFAULT_INTERPOLATION_LATENCY);

    const auto& player = m_gameController->getEntitiesModel()->getPlayer(playerID);
    std::map<uint32_t, EntitySnapshot> preShotSnapshot = m_frameCache->getFrame(rollbackLatency);
    preShotSnapshot[player->getEntityID()] = m_preRollbackState.at(player->getEntityID()); // Move shooting player forward in past snapshot
    m_gameController->getEntitiesModel()->setSnapshot(preShotSnapshot);
}

void ServerController::onDisconnected()
{
    stop();
}

void ServerController::onNodeConnected(const Net::NodeID nodeID)
{
    const int MAX_PLAYERS = 100;
    const auto& players = m_gameController->getEntitiesModel()->getPlayers();
    if (players.size() == MAX_PLAYERS)
    {
        // Server already full - let node spectate or simply kick them out?
        return;
    }
    
    const auto it = players.find(nodeID);
    if (it == players.end())  // A new challenger enters the battle!
    {
        if (nodeID != 0)
        {
            m_clientStates[nodeID] = ClientPlayerState::JOINING;
        }
        else
        {
            onPlayerJoined(0);
            m_clientStates[0] = ClientPlayerState::CONNECTED;
        }
    }

}

void ServerController::onNodeDisconnected(const Net::NodeID nodeID)
{
    CCLOG("ServerController::onNodeDisconnected %i\n", nodeID);
    m_clientStates[nodeID] = ClientPlayerState::DISCONNECTED;
    if (nodeID == 0)
    {
        stop();
    }
}

void ServerController::onPlayerJoined(const uint8_t playerID)
{
    const cocos2d::Vec2 position = cocos2d::Vec2(100 + (50 * playerID), 150);
    const float rotation = 0.f;
    const uint16_t entityID = m_gameController->getEntitiesModel()->getNextEntityID();
    m_gameController->getEntitiesModel()->incrementNextEntityID();
    auto player = m_gameController->getEntitiesController()->createPlayer(playerID,
                                                                          entityID,
                                                                          position,
                                                                          rotation);
    CCLOG("ServerController::onPlayerJoined %i, entityID: %i\n", playerID, player->getEntityID());

    const cocos2d::Size mapSize = m_levelModel->getTileMap()->getMapSize();
    const cocos2d::Size tileSize = m_levelModel->getTileMap()->getTileSize();
    float randX = cocos2d::RandomHelper::random_real(tileSize.width, (mapSize.width-1) * tileSize.width);
    float randY = cocos2d::RandomHelper::random_real(tileSize.height, (mapSize.height-1) * tileSize.height);
    cocos2d::Vec2 randomTile = cocos2d::Vec2(randX / tileSize.width, mapSize.height - (randY / tileSize.height));
    while (true)
    {
        if (!m_gameController->isTileSolid(randomTile))
        {
            break;
        }
        randX = cocos2d::RandomHelper::random_real(tileSize.width, (mapSize.width-1) * tileSize.width);
        randY = cocos2d::RandomHelper::random_real(tileSize.height, (mapSize.height-1) * tileSize.height);
        randomTile = cocos2d::Vec2(randX / tileSize.width, mapSize.height - (randY / tileSize.height));
    }

    player->setPosition(cocos2d::Vec2(randX, randY));
}

void ServerController::onClientStateMessageReceived(const std::shared_ptr<Net::Message>& data, const Net::NodeID playerID)
{
    if (auto stateMessage = std::dynamic_pointer_cast<ClientStateUpdateMessage>(data))
    {
        if (stateMessage->state == ClientState::LEVEL_LOADED)
        {
            // TODO: collect all of these messages before responding, if such a setting is set
            std::shared_ptr<ServerStartGameMessage> startGameMessage = std::make_shared<ServerStartGameMessage>();
            startGameMessage->playerID = playerID;
            std::shared_ptr<Net::Message> message = startGameMessage;
            m_networkController->sendMessage(playerID, message);
            m_clientStates[playerID] = ClientPlayerState::CONNECTED;

            const auto players = m_gameController->getEntitiesModel()->getPlayers();
            if (players.find(playerID) != players.end())
            {
                return;
            }
            onPlayerJoined(playerID);
        }
        else if (stateMessage->state == ClientState::PLAYER_RESPAWN)
        {
            if (!m_gameController->getEntitiesModel()->getPlayer(playerID))
            {
                onPlayerJoined(playerID);
            }
        }
    }
}

void ServerController::onInputMessageReceived(const std::shared_ptr<Net::Message>& data, const Net::NodeID playerID)
{
    if (auto inputMessage = std::dynamic_pointer_cast<ClientInputMessage>(data))
    {
        
        if (m_inputCache->hasReceivedSequence(playerID) &&
            m_inputCache->getLastReceivedSequence(playerID) >= inputMessage->inputSequence)
        {
            CCLOG("ServerController::onInputMessageReceived player %i, discarding input older than last (%u vs %u)",
                  playerID, inputMessage->inputSequence, m_inputCache->getLastReceivedSequence(playerID));
            return;
        }

        const float networkLatency = m_networkController->GetRoundTripTime(playerID) * 0.5f; // half because only one-way latency counts here
        const float inputGameTime = m_gameModel->getCurrentTime() - networkLatency;
        
        // Save input data
        m_inputCache->onInputDataReceived(playerID, inputMessage, inputGameTime);
    }
}

std::shared_ptr<ServerSnapshotMessage> ServerController::getFullWorldState(const Net::NodeID playerID)
{
    auto worldState = m_gameController->getEntitiesModel()->getSnapshot();
    std::shared_ptr<ServerSnapshotMessage> snapshotMessage = std::make_shared<ServerSnapshotMessage>();
    snapshotMessage->data.serverTick = m_gameModel->getCurrentTick();
    snapshotMessage->data.lastReceivedInput = m_inputCache->getLastReceivedSequence(playerID);
    snapshotMessage->data.entityCount = (uint32_t)worldState.size();
    snapshotMessage->data.entityData = worldState;

    auto players = m_gameController->getEntitiesModel()->getPlayers();
    for (auto player : players)
    {
        snapshotMessage->data.playerData[player.first].entityID = player.second->getEntityID();
        snapshotMessage->data.playerData[player.first].animationState = player.second->getAnimationState();
        snapshotMessage->data.playerData[player.first].aimPointX = player.second->getAimPosition().x;
        snapshotMessage->data.playerData[player.first].aimPointY = player.second->getAimPosition().y;
        snapshotMessage->data.playerData[player.first].health = player.second->getHealth();
        snapshotMessage->data.playerData[player.first].flipX = player.second->getFlipX();
        snapshotMessage->data.playerData[player.first].activeWeaponSlot = player.second->getActiveSlot();
        const auto& weaponSlots = player.second->getWeaponSlots();
        for (size_t i = 0; i < 5; i++)
        {
            snapshotMessage->data.playerData[player.first].weaponSlots.push_back({(uint8_t)weaponSlots.at(i).type, weaponSlots.at(i).amount});
        }
        
        if (player.first == playerID)
        {
            auto& inventory = player.second->getInventory();
            for (const auto& inventoryItem : inventory)
            {
                snapshotMessage->data.inventory.push_back({(uint8_t)inventoryItem.type, inventoryItem.amount});
            }
        }
    }
    
    return snapshotMessage;
}

std::shared_ptr<ServerSnapshotMessage> ServerController::getWorldStateDiff(const Net::NodeID playerID)
{
    // TODO: This requires us to keep track of which server tick snapshot was last confirmed received by the client node
//    const uint32_t lastClientTick = m_inputCache->getLastReceivedSequence(playerID);
    
    return nullptr;
}


void ServerController::initDebugStuff()
{
    for (int x = 1; x < 2; x++)
    {
        // Don't use dispatcher directly or local client will also catch this event
        // We want to use the network instead so we spawn the players on the server
        // as if they just joined
        size_t playerID = x;
        onPlayerJoined(playerID);
        m_botPlayers[playerID] = std::make_shared<BaseAI>();
    }
}

void ServerController::applyAI()
{
    for (auto botPlayerIt : m_botPlayers)
    {
        const uint8_t playerID = botPlayerIt.first;
        const auto& botAI = botPlayerIt.second;
        
        botAI->update(m_gameModel->getFrameTime(), playerID, m_gameController->getEntitiesModel());
        
        auto input = botAI->getInput();
        input->inputSequence = m_gameModel->getCurrentTick();
        input->lastReceivedSnapshot = m_gameModel->getCurrentTick() - 1;
        
        onInputMessageReceived(input, playerID);
    }
}
