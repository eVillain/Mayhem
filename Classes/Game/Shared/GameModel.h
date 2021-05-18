#ifndef GameModel_h
#define GameModel_h

#include "cocos2d.h"
#include "Game/Shared/GameMode.h"

class GameModel
{
public:
    GameModel();
    ~GameModel();

    void setConfig(const GameMode::Config& config);
    void reset();
    
    GameModeType getGameModeType() const { return m_gameModeType; }
    uint8_t getMaxPlayers() const { return m_maxPlayers; }
    uint8_t getPlayersPerTeam() const { return m_playersPerTeam; }
    std::string getLevel() const { return m_level; }

    void setTickRate(const uint32_t rate) { m_tickRate = rate; }
    uint32_t getTickRate() const { return m_tickRate; }

    void setCurrentTick(const uint32_t tick) { m_currentTick = tick; }
    uint32_t getCurrentTick() const { return m_currentTick; }

    void setCurrentTime(const float time) { m_currentTime = time; }
    float getCurrentTime() const { return m_currentTime; }

    void setDeltaAccumulator(const float delta) { m_deltaAccumulator = delta; }
    float getDeltaAccumulator() const { return m_deltaAccumulator; }
    
    float getFrameTime() const { return 1.f / m_tickRate; }
    
private:
    GameModeType m_gameModeType;
    uint8_t m_maxPlayers;
    uint8_t m_playersPerTeam;
    std::string m_level;
    
    uint32_t m_tickRate;
    uint32_t m_currentTick;
    float m_currentTime;
    float m_deltaAccumulator;

};

#endif /* GameModel_h */
