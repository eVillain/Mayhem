#include "GameModel.h"

#include "SharedConstants.h"

GameModel::GameModel()
: m_gameModeType()
, m_maxPlayers(8)
, m_playersPerTeam(1)
, m_tickRate(DEFAULT_TICK_RATE)
, m_currentTick(0)
, m_currentTime(0.f)
, m_deltaAccumulator(0.f)
{
    printf("GameModel:: constructor: %p\n", this);
}

GameModel::~GameModel()
{
    printf("GameModel:: destructor: %p\n", this);
}

void GameModel::setConfig(const GameMode::Config& config)
{
    m_gameModeType = config.type;
    m_tickRate = config.tickRate;
    m_maxPlayers = config.maxPlayers;
    m_playersPerTeam = config.playersPerTeam;
    m_level = config.level;
}

void GameModel::reset()
{
    m_tickRate = DEFAULT_TICK_RATE;
    m_currentTick = 0;
    m_currentTime = 0.f;
    m_deltaAccumulator = 0.f;
}
