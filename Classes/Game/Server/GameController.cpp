#include "GameController.h"

#include "EntitiesController.h"
#include "EntitiesModel.h"
#include "EntityDataModel.h"
#include "CollisionUtils.h"
#include "SharedConstants.h"
#include "Game/Shared/EntityConstants.h"

#include "Player.h"
#include "Item.h"
#include "GameModeBR.h"
#include "GameModeDM.h"
#include "AddLightEvent.h"
#include "LevelModel.h"
#include "PlayerLogic.h"

GameController::GameController(std::shared_ptr<EntitiesController> entitiesController,
                               std::shared_ptr<EntitiesModel> entitiesModel,
                               std::shared_ptr<LevelModel> levelModel)
: m_entitiesController(entitiesController)
, m_entitiesModel(entitiesModel)
, m_levelModel(levelModel)
, m_gameMode(nullptr)
, m_currentTime(0.f)
, m_host(false)
{
    printf("[Server]GameController:: constructor: %p\n", this);
}

GameController::~GameController()
{
    printf("[Server]GameController:: destructor: %p\n", this);
}

void GameController::setSnapshot(const SnapshotData& data)
{
    auto entities = m_entitiesModel->getEntities();
    for (const auto& entityData : data.entityData)
    {
        if (!entities.count(entityData.first))
        {
            CCLOG("GameController::setSnapshot missing entity %i on client!", entityData.first);
            continue;
        }

        std::shared_ptr<Entity> entity = entities.at(entityData.first);
        entity->setPosition(cocos2d::Vec2(entityData.second.positionX,
                                          entityData.second.positionY));
        entity->setRotation(entityData.second.rotation);
    }

    auto players = m_entitiesModel->getPlayers();
    for (const auto& playerData : data.playerData)
    {
        if (!players.count(playerData.first))
        {
            CCLOG("GameController::setSnapshot missing player %i on client!", playerData.first);
            continue;
        }

        std::shared_ptr<Player> player = players.at(playerData.first);
        player->setAnimationState((AnimationState)playerData.second.animationState);
        player->setAimPosition(cocos2d::Vec2(playerData.second.aimPointX, playerData.second.aimPointY));
        player->setHealth(playerData.second.health);
        player->setFlipX(playerData.second.flipX);
        player->setActiveSlot((WeaponSlot)playerData.second.activeWeaponSlot);
        auto& weaponSlots = player->getWeaponSlots();
        for (size_t i = 0; i < 5; i++)
        {
            weaponSlots.at(i).type = (EntityType)playerData.second.weaponSlots.at(i).type;
            weaponSlots.at(i).amount = playerData.second.weaponSlots.at(i).amount;
        }
    }

    const uint8_t localPlayerID = m_entitiesModel->getLocalPlayerID();
    if (!players.count(localPlayerID))
    {
        return;
    }
    std::shared_ptr<Player> player = players.at(localPlayerID);
    auto& inventory = player->getInventory();
    for (size_t i = 0; i < data.inventory.size(); i++)
    {
        if (inventory.size() > i)
        {
            inventory[i].type = (EntityType)data.inventory[i].type;
            inventory[i].amount = data.inventory[i].amount;
        }
        else
        {
            inventory.push_back({(EntityType)data.inventory[i].type, data.inventory[i].amount});
        }
    }
}

bool GameController::isTileSolid(const cocos2d::Vec2& tilePos) const
{
    return m_levelModel->isTileSolid(tilePos);
}

void GameController::applyInput(const uint8_t playerID, std::shared_ptr<ClientInputMessage>& input)
{
    if (!input)
    {
        return;
    }
 
    std::shared_ptr<Player> player = m_entitiesModel->getPlayer(playerID);
    if (!player)
    {
        return;
    }
    
    if (input->changeWeapon)
    {
        uint8_t newSlot = input->slot - 1;
        newSlot = std::max(std::min(newSlot, (uint8_t)WeaponSlot::MELEE), (uint8_t)WeaponSlot::MAIN_1);
        if (player->getActiveSlot() != newSlot)
        {
            player->setActiveSlot((WeaponSlot)newSlot);
        }
    }
    
    const cocos2d::Vec2 direction = cocos2d::Vec2(input->directionX, input->directionY);
    const cocos2d::Vec2 velocity = PlayerLogic::getMovementVelocityForInput(direction, input->run);
    bool flipX = player->getFlipX();
    const bool hasInputMovement = direction.lengthSquared() > 0.f;
    if (hasInputMovement)
    {
        flipX = input->directionX < 0.f;
    }
    player->setVelocity(velocity);
    player->setFlipX(flipX);

    cocos2d::Vec2 aimPosition = cocos2d::Vec2(input->aimPointX, input->aimPointY);
    player->setAimPosition(aimPosition);

    AnimationState animationState = AnimationState::Idle;
    
    if (checkForReload(playerID, input))
    {
        animationState = AnimationState::Reload;
    }
    else
    {
        const std::shared_ptr<Entity> entity = getEntityAtPoint(aimPosition, player->getEntityID());
        const InventoryItem& weapon = player->getWeaponSlots().at(player->getActiveSlot());
        const StaticEntityData& weaponData = EntityDataModel::getStaticEntityData(weapon.type);
        const uint16_t inventoryAmmo = player->getInventoryAmount((EntityType)weaponData.ammo.type);
        if (input->reload &&
            weapon.amount == 0 &&
            inventoryAmmo > 0)
        {
            animationState = AnimationState::Reload;
        }
        else if (input->interact &&
                 aimPosition.distance(player->getPosition()) <= ITEM_GRAB_RADIUS &&
                 std::dynamic_pointer_cast<Item>(entity))
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

    player->setAnimationState(animationState);
}

void GameController::applyInputs(const std::map<uint8_t, std::shared_ptr<ClientInputMessage>>& inputs)
{
    for (auto input : inputs)
    {
        applyInput(input.first, input.second);
    }
}

void GameController::tick(const float deltaTime)
{
    m_currentTime += deltaTime;
    m_gameMode->update(deltaTime);
    m_entitiesController->update(deltaTime);
}

void GameController::setGameMode(const GameMode::Config& config, const bool host)
{
    m_host = host;

    if (config.type == GameModeType::GAME_MODE_BATTLEROYALE)
    {
        m_gameMode = std::make_shared<GameModeBR>(m_entitiesController, m_entitiesModel, m_levelModel, 1337);
    }
    else
    {
        m_gameMode = std::make_shared<GameModeDM>(m_entitiesController, m_entitiesModel, m_levelModel, 1337);
    }
    
    m_gameMode->onLevelLoaded(host);
}

std::shared_ptr<Entity> GameController::getEntityAtPoint(const cocos2d::Vec2& point,
                                                         const uint32_t ignoreEntityID) const
{
    auto entities = m_entitiesModel->getEntities();
    for (const auto& entityPair : entities)
    {
        if (entityPair.first == ignoreEntityID)
        {
            continue;
        }
        const std::shared_ptr<Entity>& entity = entityPair.second;
        const auto& collisionRects = EntityDataModel::getCollisionRects(entityPair.second->getEntityType());
        for (const auto& baseRect : collisionRects)
        {
            const cocos2d::Rect rect = cocos2d::Rect(baseRect.origin + entity->getPosition(), baseRect.size);
            if (rect.containsPoint(point))
            {
                return entity;
            }
        }
    }
    return nullptr;
}

const bool GameController::checkForReload(uint8_t playerID, const std::shared_ptr<ClientInputMessage>& input)
{
    auto player = m_entitiesModel->getPlayer(playerID);
    if (player->isReloading(m_currentTime))
    {
        return true;
    }
    
    if (input->reload && player->canReload(m_currentTime))
    {
        player->setLastActionTime(m_currentTime);
        return true;
    }
    
    InventoryItem& weapon = player->getWeaponSlots().at(player->getActiveSlot());
    if (weapon.type == EntityType::PlayerEntity)
    {
        return false;
    }
    
    const auto& itemData = EntityDataModel::getStaticEntityData(weapon.type);
    const uint16_t inventoryAmmo = player->getInventoryAmount((EntityType)itemData.ammo.type);
    const uint16_t ammo = weapon.amount;
    if (player->getAnimationState() == AnimationState::Reload)
    {
        const bool reloadFinished = player->getLastActionTime() + DEFAULT_RELOAD_TIME < m_currentTime;
        if (reloadFinished)
        {
            const uint16_t roomInMagazine = itemData.ammo.amount - ammo;
            const uint16_t ammoMove = std::min(roomInMagazine, inventoryAmmo);
            weapon.amount += ammoMove;
            player->setInventoryAmount((EntityType)itemData.ammo.type, inventoryAmmo - ammoMove);
            player->setLastActionTime(-1);
            return false;
        }
        return true;
    }
    return false;
}
