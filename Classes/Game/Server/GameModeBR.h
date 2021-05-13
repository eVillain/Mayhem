#ifndef GameModeBR_h
#define GameModeBR_h

#include "GameMode.h"

class GameModeBR : public GameMode
{
public:
    GameModeBR(std::shared_ptr<EntitiesController> entitiesController,
               std::shared_ptr<EntitiesModel> entitiesModel,
               std::shared_ptr<LevelModel> levelModel,
               const int seed);

    const GameModeType getType() override { return GameModeType::GAME_MODE_BATTLEROYALE; }
    
    void update(const float deltaTime) override;
    bool allowLateJoin() const override { return false; }
    bool allowRespawn() const override { return false; }
    
    void onLevelLoaded(const bool isHost) override;
    void onPlayerReady(const uint8_t playerID) override;
    void onPlayerDied(const uint8_t playerID) override;

private:
    const int m_seed;
    float m_tileUpdateAccumulator;
    int m_tileUpdateLayer;
    std::vector<cocos2d::Vec2> m_deadTiles;
    
    void spawnThings();
    void fillDeadTiles();
    
    void removeNextDeadTile();
};

#endif /* GameModeBR_h */
