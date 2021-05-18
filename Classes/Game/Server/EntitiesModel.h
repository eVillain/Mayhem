#ifndef EntitiesModel_h
#define EntitiesModel_h

#include <set>
#include <memory>
#include <list>
#include "Network/NetworkMessages.h"
#include "Game/Shared/EntityConstants.h"


class Entity;
class Player;

class EntitiesModel
{
public:   
    EntitiesModel();
    ~EntitiesModel();
    
    void addEntity(const uint32_t entityID, const std::shared_ptr<Entity>& entity);
    void removeEntity(const uint32_t entityID);
    
    void removePlayer(const uint8_t playerID);
    
    std::map<uint32_t, std::shared_ptr<Entity>>& getEntities() { return m_entities; }
    std::map<uint8_t, std::shared_ptr<Player>>& getPlayers() { return m_players; }

    const std::shared_ptr<Player> getPlayer(const uint8_t playerID);
    void setPlayer(const uint8_t playerID, const std::shared_ptr<Player>& player) { m_players[playerID] = player; }
    
    const std::shared_ptr<Player> getPlayerByEntityID(const uint16_t entityID);
    const uint8_t getPlayerIDByEntityID(const uint16_t entityID);

    uint8_t getLocalPlayerID() const { return m_localPlayerID; }
    void setLocalPlayerID(const uint8_t playerID) { m_localPlayerID = playerID; }

    const std::shared_ptr<Player>& getLocalPlayer() { return m_players.at(m_localPlayerID); }
    
    uint32_t getNextEntityID() const { return m_nextEntityID; }
    void setNextEntityID(const uint32_t nextID) { m_nextEntityID = nextID; }
    uint32_t incrementNextEntityID() { return ++m_nextEntityID; }

    void setSnapshot(const std::map<uint32_t, EntitySnapshot>& snapshot);
    std::map<uint32_t, EntitySnapshot> getSnapshot() const;
    std::map<uint32_t, EntitySnapshot> getDiff(const std::map<uint32_t, EntitySnapshot>& snapshot) const;

    const std::vector<std::shared_ptr<Entity>> getEntitiesNearPosition(const cocos2d::Vec2 position, const float radius) const;
    const std::vector<std::shared_ptr<Player>> getPlayersNearPosition(const cocos2d::Vec2 position, const float radius) const;

private:
    std::map<uint32_t, std::shared_ptr<Entity>> m_entities;
    std::map<uint8_t, std::shared_ptr<Player>> m_players;
    
    uint32_t m_nextEntityID;
    uint8_t m_localPlayerID;
};

#endif /* EntitiesModel_h */
