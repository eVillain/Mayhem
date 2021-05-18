#include "BaseAI.h"
#include "NetworkMessages.h"
#include "EntitiesModel.h"
#include "EntityDataModel.h"
#include "Player.h"
#include "SharedConstants.h"

const float BaseAI::AI_UPDATE_INTERVAL = 1.f;
const float BaseAI::AI_AWARENESS_RADIUS = 300.f;

BaseAI::BaseAI()
: m_state(MOVE_TO_TARGET)
, m_targetType(NONE)
, m_updateAccumulator(0.f)
, m_updateTime(1.f)
, m_directionX(0.f)
, m_directionY(0.f)
, m_aimPointX(0.f)
, m_aimPointY(0.f)
, m_shoot(false)
, m_interact(false)
, m_run(false)
, m_reload(false)
, m_changeWeapon(false)
, m_slot(0)
{
}

void BaseAI::update(const float deltaTime,
                    const uint8_t playerID,
                    const std::shared_ptr<EntitiesModel>& entitiesModel)
{
    const auto& players = entitiesModel->getPlayers();
    auto playerIt = players.find(playerID);
    if (playerIt == players.end())
    {
        return; // dead bot, respawn?
    }
    
    m_updateAccumulator += deltaTime;
    
    if (m_updateAccumulator >= m_updateTime)
    {
        m_updateAccumulator = 0.f;
        
        refreshState(playerID, entitiesModel);
    }
    
    updateState(playerID, entitiesModel);
}

std::shared_ptr<ClientInputMessage> BaseAI::getInput() const
{
    auto input = std::make_shared<ClientInputMessage>();
    
    input->directionX = m_directionX;
    input->directionY = m_directionY;
    input->aimPointX = m_aimPointX;
    input->aimPointY = m_aimPointY;
    input->shoot = m_shoot;
    input->interact = m_interact;
    input->run = m_run;
    input->reload = m_reload;
    input->changeWeapon = m_changeWeapon;
    input->slot = m_slot;
    input->pickUpType = 0;
    input->pickUpID = 0;
    input->pickUpAmount = 0;
    
    return input;
}


// Find current objective based on current state
// Basic bot is basic: simulates a noob player with minimal understanding of the game
// Priorities should be as follows:
// 1 - Have weapon, have ammo, have ammo loaded in weapon
// 2 - Find enemy, kill enemy
// This breaks our actions down to 2 sets:
// 1 - Move somewhere
// 2 - Do an action (pickup weapon/ammo, reload, shoot)
void BaseAI::refreshState(const uint8_t playerID,
                          const std::shared_ptr<EntitiesModel>& entitiesModel)
{
    const auto& players = entitiesModel->getPlayers();
    auto playerIt = players.find(playerID);
    if (playerIt == players.end())
    {
        return;
    }

    const auto& botPlayer = playerIt->second;
    const cocos2d::Vec2 position = botPlayer->getPosition();
    const InventoryItem& weapon = botPlayer->getWeaponSlots().at(botPlayer->getActiveSlot());
    const auto nearbyPlayers = entitiesModel->getPlayersNearPosition(position, AI_AWARENESS_RADIUS);
    const bool areThreatsNearby = nearbyPlayers.size() > 1;
    const bool seekWeapon = weapon.type == EntityType::PlayerEntity;
    const bool needsReload = weapon.amount == 0;

    const auto& itemData = EntityDataModel::getStaticEntityData(weapon.type);
    const uint16_t inventoryAmmo = botPlayer->getInventoryAmount((EntityType)itemData.ammo.type);
    
    m_targetType = TargetType::NONE;
    if (areThreatsNearby && (seekWeapon || needsReload))
    {
        m_targetType = TargetType::THREAT;
        m_state = State::MOVE_TO_TARGET;
        m_updateTime = 1.f;
    }
    else if (seekWeapon)
    {
        m_targetType = TargetType::WEAPON;
        m_state = State::MOVE_TO_TARGET;
        m_updateTime = 0.5f;
    }
    else
    {
        if (needsReload)
        {
            m_targetType = TargetType::AMMO;
            if (inventoryAmmo > 0)
            {
                m_state = State::PERFORM_ACTION;
                m_updateTime = 0.5f;
            }
            else
            {
                m_state = State::MOVE_TO_TARGET;
                m_updateTime = 1.f;
            }
        }
        else
        {
            m_targetType = TargetType::ENEMY;
            m_state = State::MOVE_TO_TARGET;
            m_updateTime = 0.3f;
        }
    }
}

void BaseAI::updateState(const uint8_t playerID,
                         const std::shared_ptr<EntitiesModel>& entitiesModel)
{
    resetState();
    
    const auto& players = entitiesModel->getPlayers();
    auto playerIt = players.find(playerID);
    if (playerIt == players.end())
    {
        return;
    }

    const auto& botPlayer = playerIt->second;
    const cocos2d::Vec2 position = botPlayer->getPosition();
    const InventoryItem& weapon = botPlayer->getWeaponSlots().at(botPlayer->getActiveSlot());
    const auto& itemData = EntityDataModel::getStaticEntityData(weapon.type);
    const auto nearbyPlayers = entitiesModel->getPlayersNearPosition(position, AI_AWARENESS_RADIUS);
//    const bool areThreatsNearby = nearbyPlayers.size() > 1;
//    const bool seekWeapon = weapon.type == EntityType::PlayerEntity;
//    const bool needsReload = weapon.amount == 0;
//    const uint16_t inventoryAmmo = botPlayer->getInventoryAmount((EntityType)itemData.ammo.type);
    
    if (m_state == State::MOVE_TO_TARGET)
    {
        if (m_targetType == TargetType::NONE)
        {
            const float distSQ = position.distanceSquared(cocos2d::Vec2(m_aimPointX, m_aimPointY));
            if (distSQ < 10.f || (m_aimPointX == 0.f && m_aimPointY == 0.f))
            {
                static const std::vector<cocos2d::Vec2> DIRECTIONS = {
                    cocos2d::Vec2(-1,0), cocos2d::Vec2(0, 1), cocos2d::Vec2(1, 0), cocos2d::Vec2(0, -1)
                };
                const int random = cocos2d::random(0, 3);
                // Move to random position
                m_directionX = DIRECTIONS.at(random).x;
                m_directionY = DIRECTIONS.at(random).y;
                
                m_aimPointX = position.x + m_directionX * 10.f;
                m_aimPointY = position.y + m_directionY * 10.f;
            }
            else
            {
                cocos2d::Vec2 direction = cocos2d::Vec2(m_aimPointX, m_aimPointY) - position;
                direction.normalize();
                m_aimPointX = direction.x;
                m_aimPointY = direction.y;
            }
            return;
        }
        // Seek target
        const auto nearEntities = entitiesModel->getEntitiesNearPosition(position, AI_AWARENESS_RADIUS);
        
        cocos2d::Vec2 targetPos = getClosestOfType(m_targetType,
                                                   position,
                                                   botPlayer->getEntityID(),
                                                   nearEntities,
                                                   (EntityType)itemData.ammo.type);

        m_aimPointX = targetPos.x;
        m_aimPointY = targetPos.y;

        if (m_targetType == TargetType::THREAT)
        {
            cocos2d::Vec2 direction = (position - targetPos).getNormalized();
            m_directionX = direction.x;
            m_directionY = -direction.y;
        }
        else
        {
            const cocos2d::Vec2 targetDelta = (targetPos - position);
            const float distToTarget = targetDelta.length();
            m_directionX = targetDelta.getNormalized().x;
            m_directionY = -targetDelta.getNormalized().y;

            if (distToTarget < ITEM_GRAB_RADIUS &&
                (m_targetType == TargetType::AMMO || m_targetType == TargetType::WEAPON))
            {
                m_state = State::PERFORM_ACTION;
            }
            else if (distToTarget < 50.f &&
                (m_targetType == TargetType::ENEMY))
            {
                m_state = State::PERFORM_ACTION;
            }
        }
    }
    else
    {
        // Seek target
        const auto nearEntities = entitiesModel->getEntitiesNearPosition(position, AI_AWARENESS_RADIUS);
        
        cocos2d::Vec2 targetPos = getClosestOfType(m_targetType,
                                                   position,
                                                   botPlayer->getEntityID(),
                                                   nearEntities,
                                                   (EntityType)itemData.ammo.type);

        m_aimPointX = targetPos.x;
        m_aimPointY = targetPos.y;
        
        if (m_targetType == TargetType::AMMO ||
            m_targetType == TargetType::WEAPON)
        {
            m_interact = true; // Grab the thing
            m_reload = true;
        }
        else if (m_targetType == TargetType::ENEMY)
        {
            const cocos2d::Vec2 targetDelta = (targetPos - position);
            const float distToTarget = targetDelta.length();
            if (distToTarget < 50.f)
            {
                // Miss by a little bit
                const float AI_AIM_MISS_RANGE = 20.f;
                float randomX = cocos2d::random(-AI_AIM_MISS_RANGE, AI_AIM_MISS_RANGE);
                float randomY = cocos2d::random(-AI_AIM_MISS_RANGE, AI_AIM_MISS_RANGE);
                m_aimPointX += randomX;
                m_aimPointY += randomY;
                m_shoot = true;
            }
            else
            {
                m_state = State::MOVE_TO_TARGET;
            }
        }
    }
}

void BaseAI::resetState()
{
    m_directionX = 0.f;
    m_directionY = 0.f;
    m_shoot = false;
    m_interact = false;
    m_run = false;
    m_reload = false;
    m_changeWeapon = false;
}

cocos2d::Vec2 BaseAI::getClosestOfType(const TargetType type,
                                       const cocos2d::Vec2& position,
                                       const uint16_t ignoreEntityID,
                                       const std::vector<std::shared_ptr<Entity>>& entities,
                                       EntityType ammoType) const
{
    cocos2d::Vec2 closestPosition = position;
    float closestDistanceSQ = AI_AWARENESS_RADIUS * AI_AWARENESS_RADIUS;
    for (const auto& entity : entities)
    {
        if (entity->getEntityID() == ignoreEntityID)
        {
            continue;
        }
        const EntityType nearEntityType = entity->getEntityType();
        
        bool isCorrectType = false;
        if (type == TargetType::WEAPON && EntityDataModel::isWeaponType(nearEntityType))
        {
            isCorrectType = true;
        }
        else if (type == TargetType::AMMO && nearEntityType == ammoType)
        {
            isCorrectType = true;
        }
        else if ((type == TargetType::ENEMY || type == TargetType::THREAT) && nearEntityType == EntityType::PlayerEntity)
        {
            isCorrectType = true;
        }
        
        if (isCorrectType)
        {
            const float distSQ = position.distanceSquared(entity->getPosition());
            if (distSQ < closestDistanceSQ)
            {
                closestDistanceSQ = distSQ;
                closestPosition = entity->getPosition();
            }
        }
    }
    return closestPosition;
}
