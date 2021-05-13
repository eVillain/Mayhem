#ifndef GameModel_h
#define GameModel_h

#include "cocos2d.h"

class GameModel
{
public:
    GameModel();
    
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
    uint32_t m_tickRate;
    uint32_t m_currentTick;
    float m_currentTime;
    float m_deltaAccumulator;

};

#endif /* GameModel_h */
