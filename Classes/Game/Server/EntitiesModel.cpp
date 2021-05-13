#include "EntitiesModel.h"
#include "Entity.h"
#include "Player.h"

EntitiesModel::EntitiesModel()
: m_nextEntityID(0)
, m_localPlayerID(0)
{
}

EntitiesModel::~EntitiesModel()
{
}

void EntitiesModel::addEntity(const uint32_t entityID, const std::shared_ptr<Entity>& entity)
{
    m_entities[entityID] = entity;
}

void EntitiesModel::removeEntity(const uint32_t entityID)
{
    m_entities.erase(entityID);
}

void EntitiesModel::removePlayer(const uint8_t playerID)
{
    m_players.erase(playerID);
}

void EntitiesModel::setSnapshot(const std::map<uint32_t, EntitySnapshot>& snapshot)
{
    for (auto entityPair : m_entities)
    {
        if (!snapshot.count(entityPair.first))
        {
            continue;
        }
        const cocos2d::Vec2 position = cocos2d::Vec2(snapshot.at(entityPair.first).positionX,
                                                     snapshot.at(entityPair.first).positionY);
        const cocos2d::Vec2 velocity = cocos2d::Vec2(snapshot.at(entityPair.first).velocityX,
                                                     snapshot.at(entityPair.first).velocityY);
        entityPair.second->setPosition(position);
        entityPair.second->setRotation(snapshot.at(entityPair.first).rotation);
        entityPair.second->setVelocity(velocity);
        entityPair.second->setAngularVelocity(snapshot.at(entityPair.first).angularVelocity);
    }
}

std::map<uint32_t, EntitySnapshot> EntitiesModel::getSnapshot() const
{
    std::map<uint32_t, EntitySnapshot> snapshot;
    for (auto entityPair : m_entities)
    {
        snapshot[entityPair.first] = {
            entityPair.second->getPosition().x,
            entityPair.second->getPosition().y,
            entityPair.second->getRotation(),
            entityPair.second->getVelocity().x,
            entityPair.second->getVelocity().y,
            entityPair.second->getAngularVelocity(),
            (uint8_t)entityPair.second->getEntityType()
        };
    }
    
    return snapshot;
}

std::map<uint32_t, EntitySnapshot> EntitiesModel::getDiff(const std::map<uint32_t, EntitySnapshot>& snapshot) const
{
    std::map<uint32_t, EntitySnapshot> diff;

    for (auto entityPair : m_entities)
    {
        auto it = snapshot.find(entityPair.first);
        if (it == snapshot.end())
        {
            // New entity
            diff[entityPair.first] = {
                entityPair.second->getPosition().x,
                entityPair.second->getPosition().y,
                entityPair.second->getRotation(),
                entityPair.second->getVelocity().x,
                entityPair.second->getVelocity().y,
                entityPair.second->getAngularVelocity(),
                (uint8_t)entityPair.second->getEntityType()
            };
        }
        else
        {
            // Store delta
            auto snapshotEntity = it->second;
            diff[entityPair.first] = {
                entityPair.second->getPosition().x - snapshotEntity.positionX,
                entityPair.second->getPosition().y - snapshotEntity.positionY,
                entityPair.second->getRotation() - snapshotEntity.rotation,
                entityPair.second->getVelocity().x - snapshotEntity.velocityX,
                entityPair.second->getVelocity().y - snapshotEntity.velocityY,
                entityPair.second->getAngularVelocity() - snapshotEntity.angularVelocity,
                (uint8_t)entityPair.second->getEntityType()
            };
        }
    }
    
    return diff;
}


const std::shared_ptr<Player> EntitiesModel::getPlayer(const uint8_t playerID)
{
    auto it = m_players.find(playerID);
    if (it != m_players.end())
    {
        return it->second;
    }
    return nullptr;
}

const std::shared_ptr<Player> EntitiesModel::getPlayerByEntityID(const uint16_t entityID)
{
    auto it = std::find_if(m_players.begin(), m_players.end(), [entityID](const std::pair<uint8_t, const std::shared_ptr<Player>>& player) {
        return player.second->getEntityID() == entityID;
    });
    if (it != m_players.end())
    {
        return it->second;
    }
    return nullptr;
}

const std::vector<std::shared_ptr<Entity>> EntitiesModel::getEntitiesNearPosition(const cocos2d::Vec2 position,
                                                                                  const float radius) const
{
    std::vector<std::shared_ptr<Entity>> nearEntities;
    const float radiusSq = radius * radius;
    for (auto entityPair : m_entities)
    {
        if (entityPair.second->getPosition().distanceSquared(position) <= radiusSq)
        {
            nearEntities.push_back(entityPair.second);
        }
    }
    
    return nearEntities;
}

const std::vector<std::shared_ptr<Player>> EntitiesModel::getPlayersNearPosition(const cocos2d::Vec2 position,
                                                                                 const float radius) const
{
    std::vector<std::shared_ptr<Player>> nearPlayers;
    const float radiusSq = radius * radius;
    for (auto playerPair : m_players)
    {
        const int16_t entityID = playerPair.second->getEntityID();
        auto entity = m_entities.at(entityID);
        if (entity->getPosition().distanceSquared(position) <= radiusSq)
        {
            nearPlayers.push_back(playerPair.second);
        }
    }
    
    return nearPlayers;
}
