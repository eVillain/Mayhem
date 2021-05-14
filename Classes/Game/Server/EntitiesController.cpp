#include "EntitiesController.h"

#include "EntitiesModel.h"
#include "EntityDataModel.h"
#include "Entity.h"
#include "Player.h"
#include "Item.h"
#include "LootBox.h"
#include "Projectile.h"

EntitiesController::EntitiesController(std::shared_ptr<EntitiesModel> entitiesModel)
: m_model(entitiesModel)
{
}

EntitiesController::~EntitiesController()
{
}

void EntitiesController::initialize()
{
}

void EntitiesController::update(const float deltaTime)
{
    removeDeadEntities();
}

void EntitiesController::removeDeadEntities()
{
    for (auto deadEntity : m_deadEntities)
    {
        if (m_model->getEntities().count(deadEntity))
        {
            auto entity = m_model->getEntities()[deadEntity];
            m_model->removeEntity(deadEntity);
        }
        else
        {
            cocos2d::log("EntitiesController error: dead entity %u not found in entities!", deadEntity);
            assert(false);
        }
    }
    m_deadEntities.clear();
}

std::shared_ptr<Player> EntitiesController::createPlayer(const uint8_t playerID,
                                                         const uint16_t entityID,
                                                         const cocos2d::Vec2& position,
                                                         const float rotation)
{
    std::shared_ptr<Player> player = std::make_shared<Player>(entityID);
    player->setPlayerID(playerID);
    player->setPosition(position);
    player->setRotation(rotation);

    m_model->addEntity(entityID, player);
    m_model->setPlayer(playerID, player);

    return player;
}

std::shared_ptr<Item> EntitiesController::createItem(const uint16_t entityID,
                                                     const EntityType type,
                                                     const cocos2d::Vec2& position,
                                                     const float rotation)
{
    std::shared_ptr<Item> item;
    int amount = 1;

    if (type == EntityType::Loot_Box)
    {
        item = std::make_shared<LootBox>(entityID);
    }
    else
    {
        item = std::make_shared<Item>(entityID, type);
    }
    
    item->setPosition(position);
    item->setRotation(rotation);
    if (type >= EntityType::Item_Ammo_9mm &&
        type <= EntityType::Item_Ammo_Slugs)
    {
        auto itemData = EntityDataModel::getStaticEntityData(type);
        amount = itemData.ammo.amount;
    }
    else if (type >= EntityType::Item_Deagle &&
             type <= EntityType::Item_Kar98 )
    {
        auto itemData = EntityDataModel::getStaticEntityData(type);
        amount = itemData.ammo.amount;
    }

    item->setAmount(amount);
    m_model->addEntity(entityID, item);

    return item;
}

std::shared_ptr<Projectile> EntitiesController::createProjectile(const uint16_t entityID,
                                                                 const EntityType type,
                                                                 const cocos2d::Vec2& position,
                                                                 const cocos2d::Vec2& velocity,
                                                                 const float rotation)
{
    std::shared_ptr<Projectile> projectile = std::make_shared<Projectile>(entityID, type);
    projectile->setPosition(position);
    projectile->setRotation(rotation);
    projectile->setVelocity(velocity);
    
    m_model->addEntity(entityID, projectile);
    
    return projectile;
}

void EntitiesController::destroyEntity(const uint16_t entityID)
{
    m_deadEntities.push_back(entityID);
}
