#include "GameModel.h"

#include "SharedConstants.h"

GameModel::GameModel()
: m_tickRate(DEFAULT_TICK_RATE)
, m_currentTick(0)
, m_currentTime(0.f)
, m_deltaAccumulator(0.f)
{
}

void GameModel::reset()
{
    m_tickRate = DEFAULT_TICK_RATE;
    m_currentTick = 0;
    m_currentTime = 0.f;
    m_deltaAccumulator = 0.f;
}
