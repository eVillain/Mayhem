#ifndef GameMode_h
#define GameMode_h

#include "cocos2d.h"
#include <map>
#include <vector>

class EntitiesController;
class EntitiesModel;
class LevelModel;
class ViewLayer;

enum GameModeType {
    GAME_MODE_DEATHMATCH,
    GAME_MODE_BATTLEROYALE,
    GAME_MODE_COUNT,
};

class GameMode
{
public:
    struct Config {
        GameModeType type;
        uint32_t tickRate;
        uint8_t maxPlayers;
        uint8_t playersPerTeam;
        std::string level;
    };
    
    struct TeamData {
        std::vector<uint8_t> players;
    };
    
    static const std::string getGameModeName(const GameModeType type)
    {
        std::string name;
        switch (type)
        {
            case GAME_MODE_DEATHMATCH:
                name = "Deathmatch";
                break;
            case GAME_MODE_BATTLEROYALE:
                name = "Battle Royale";
                break;
            default:
                name = "Unknown GameMode";
                break;
        }
        return name;
    }
    
    GameMode(std::shared_ptr<EntitiesController> entitiesController,
             std::shared_ptr<EntitiesModel> entitiesModel,
             std::shared_ptr<LevelModel> levelModel);

    virtual const GameModeType getType() = 0;
    
    virtual void update(const float deltaTime) = 0;
    virtual bool allowLateJoin() const = 0;
    virtual bool allowRespawn() const = 0;

    virtual void onLevelLoaded(const bool isHost) = 0;
    virtual void onPlayerReady(const uint8_t playerID) = 0;
    virtual void onPlayerDied(const uint8_t playerID) = 0;
    virtual void onPlayerGotAKill(const uint8_t playerID) = 0;

    uint8_t getPlayersAlive() const;
    uint8_t getTeamsAlive() const;
    uint16_t getPlayerKills(const uint8_t playerID) const;
    uint8_t getMaxPlayers() const { return m_maxPlayers; }
    uint8_t getPlayersPerTeam() const { return m_playersPerTeam; }
    void setPlayersPerTeam(uint8_t players) { m_playersPerTeam = players; }
    void setPlayerTeam(const uint8_t playerID, const uint8_t teamID) { m_teams[teamID].players.push_back(playerID); }
    void setTileDeathCallback(std::function<void(int, int)> cb) { m_tileDeathCallback = cb; }
    void setWinConditionReachedCallback(std::function<void(uint8_t)> cb) { m_winConditionCallback = cb; }

protected:
    std::shared_ptr<EntitiesController> m_entitiesController;
    std::shared_ptr<EntitiesModel> m_entitiesModel;
    std::shared_ptr<LevelModel> m_levelModel;

    uint8_t m_maxPlayers;
    uint8_t m_playersPerTeam;
    std::vector<TeamData> m_teams;
    std::map<uint8_t, uint32_t> m_playerKills;
    std::map<uint8_t, bool> m_playerStates;

    std::function<void(int, int)> m_tileDeathCallback; // Tile x, y coords as params
    std::function<void(uint8_t)> m_winConditionCallback; // Winning team / winning player as param
};

#endif /* GameMode_h */
