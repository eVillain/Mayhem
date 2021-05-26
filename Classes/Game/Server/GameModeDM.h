#ifndef GameModeDM_h
#define GameModeDM_h

#include "Game/Shared/GameMode.h"
#include <stdint.h>

class GameModeDM : public GameMode
{
public:
    GameModeDM(std::shared_ptr<EntitiesController> entitiesController,
               std::shared_ptr<EntitiesModel> entitiesModel,
               std::shared_ptr<LevelModel> levelModel,
               const int seed);

    const GameModeType getType() override { return GameModeType::GAME_MODE_DEATHMATCH; }

    void update(const float deltaTime) override;
    bool allowLateJoin() const override { return true; }
    bool allowRespawn() const override { return true; }
    
    void onLevelLoaded(const bool isHost) override;
    void onPlayerReady(const uint8_t playerID) override;
    void onPlayerDied(const uint8_t playerID) override;
    void onPlayerGotAKill(const uint8_t playerID) override;
    
private:
    const int m_seed;
    const uint32_t m_maxKills;
    const float m_maxTime;
    float m_time;
        
    void spawnThings();
    void spawnRandomGunWithAmmo(const cocos2d::Vec2& position);
};

#endif /* GameModeDM_h */
