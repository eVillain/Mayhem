#include "GameMode.h"

GameMode::GameMode(std::shared_ptr<EntitiesController> entitiesController,
                   std::shared_ptr<EntitiesModel> entitiesModel,
                   std::shared_ptr<LevelModel> levelModel)
: m_entitiesController(entitiesController)
, m_entitiesModel(entitiesModel)
, m_levelModel(levelModel)
, m_maxPlayers(2)
, m_playersPerTeam(1)
, m_tileDeathCallback(nullptr)
, m_winConditionCallback(nullptr)
{
}

uint8_t GameMode::getPlayersAlive() const
{
    uint8_t playersAlive = 0;
    for (const auto& pair : m_playerStates)
    {
        if (pair.second)
        {
            playersAlive++;
        }
    }
    return playersAlive;
}

uint8_t GameMode::getTeamsAlive() const
{
    uint8_t teamsAlive = 0;
    for (const auto& team : m_teams)
    {
        for (const auto& player : team.players)
        {
            const auto& it = m_playerStates.find(player);
            if (it != m_playerStates.end() &&
                it->second)
            {
                teamsAlive++;
                break;
            }
        }
    }
    return teamsAlive;
}

uint16_t GameMode::getPlayerKills(const uint8_t playerID) const
{
    if (m_playerKills.find(playerID) == m_playerKills.end())
    {
        return 0;
    }
    return m_playerKills.at(playerID);
}
