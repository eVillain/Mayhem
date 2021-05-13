#include "BaseAI.h"
#include "NetworkMessages.h"
#include "EntitiesModel.h"
#include "EntityDataModel.h"
#include "Player.h"
#include "SharedConstants.h"

BaseAI::BaseAI()
: m_state(SEEK_TARGET)
, m_targetType(NONE)
, m_updateAccumulator(0.f)
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
    const float AI_UPDATE_INTERVAL = 1.f;
    m_updateAccumulator += deltaTime;
    
    while (m_updateAccumulator > AI_UPDATE_INTERVAL)
    {
        updateState(playerID, entitiesModel);
        
        m_updateAccumulator -= AI_UPDATE_INTERVAL;
    }
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

void BaseAI::updateState(const uint8_t playerID,
                         const std::shared_ptr<EntitiesModel>& entitiesModel)
{
    const float AI_AWARENESS_RADIUS = 200.f;
    m_directionX = 0.f;
    m_directionY = 0.f;
    m_aimPointX = 0.f;
    m_aimPointY = 0.f;
    m_shoot = false;
    m_interact = false;
    m_run = false;
    m_reload = false;
    m_changeWeapon = false;
    m_targetType = TargetType::NONE;
    
    const auto& players = entitiesModel->getPlayers();
    auto playerIt = players.find(playerID);
    if (playerIt == players.end())
    {
        return; // dead bot, respawn?
    }
    
    // Find current objective based on current state
    // Basic bot is basic: simulates a noob player with minimal understanding of the game
    // Priorities should be as follows:
    // 1 - Have weapon, have ammo, have ammo loaded in weapon
    // 2 - Find enemy, kill enemy
    // This breaks our actions down to 2 sets:
    // 1 - Move somewhere
    // 2 - Do an action (pickup weapon/ammo, reload, shoot)
    const auto& botPlayer = playerIt->second;
    const cocos2d::Vec2 position = botPlayer->getPosition();
    const InventoryItem& weapon = botPlayer->getWeaponSlots().at(botPlayer->getActiveSlot());
    const auto& itemData = EntityDataModel::getStaticEntityData(weapon.type);
    const bool seekWeapon = weapon.type == EntityType::PlayerEntity;
    if (seekWeapon)
    {
        m_targetType = TargetType::WEAPON;
    }
    else
    {
        const bool needsReload = weapon.amount == 0;
        const uint16_t inventoryAmmo = botPlayer->getInventoryAmount((EntityType)itemData.ammo.type);
        if (needsReload && inventoryAmmo > 0)
        {
            m_reload = true;
        }
        else
        {
            const bool seekAmmo = needsReload && inventoryAmmo == 0;
            if (seekAmmo)
            {
                m_targetType = TargetType::AMMO;
            }
            else
            {
                m_targetType = TargetType::ENEMY;
            }
        }
    }

    if (m_targetType != TargetType::NONE)
    {
        // Seek target
        cocos2d::Vec2 targetPos;
        bool targetFound = false;
        const auto nearEntities = entitiesModel->getNearEntities(position, AI_AWARENESS_RADIUS);
        for (const auto& entity : nearEntities)
        {
            if (entity->getEntityID() == botPlayer->getEntityID())
            {
                continue; // Skip self :)
            }
            const EntityType nearEntityType = entity->getEntityType();
            if (m_targetType == TargetType::WEAPON &&
                EntityDataModel::isWeaponType(nearEntityType))
            {
                targetPos = entity->getPosition();
                targetFound = true;
                break;
            }
            else if (m_targetType == TargetType::AMMO &&
                     nearEntityType == (EntityType)itemData.ammo.type)
            {
                targetPos = entity->getPosition();
                targetFound = true;
                break;
            }
            else if (m_targetType == TargetType::ENEMY &&
                     nearEntityType == EntityType::PlayerEntity)
            {
                targetPos = entity->getPosition();
                targetFound = true;
                break;
            }
        }
        
        // Move or do action
        if (targetFound)
        {
            m_aimPointX = targetPos.x;
            m_aimPointY = targetPos.y;
            
            const cocos2d::Vec2 targetDelta = (targetPos - position);
            const float distToTarget = targetDelta.length();
            if (distToTarget < ITEM_GRAB_RADIUS &&
                (m_targetType == TargetType::AMMO || m_targetType == TargetType::WEAPON))
            {
                m_interact = true; // Grab the thing
            }
            else
            {
                // Move toward target
                m_directionX = targetDelta.getNormalized().x;
                m_directionY = targetDelta.getNormalized().y;
                if (m_targetType == TargetType::ENEMY)
                {
                    // Miss by a little bit
                    const float AI_AIM_MISS_RANGE = 20.f;
                    float randomX = cocos2d::random(-AI_AIM_MISS_RANGE, AI_AIM_MISS_RANGE);
                    float randomY = cocos2d::random(-AI_AIM_MISS_RANGE, AI_AIM_MISS_RANGE);
                    m_aimPointX += randomX;
                    m_aimPointY += randomY;
                    m_shoot = true;
                }
            }
        }
        else
        {
            // Move to random position
            float randomX = cocos2d::random(-1.f, 1.f);
            float randomY = cocos2d::random(-1.f, 1.f);
            m_directionX = randomX;
            m_directionY = randomY;
        }
    }
}
