#ifndef GameController_h
#define GameController_h

#include "Core/Event.h"
#include "Network/NetworkMessages.h"
#include "Game/Shared/GameMode.h"

class EntitiesController;
class EntitiesModel;
class LevelModel;
class Entity;

class GameController
{
public:
    GameController(std::shared_ptr<EntitiesController> entitiesController,
                   std::shared_ptr<EntitiesModel> entitiesModel,
                   std::shared_ptr<LevelModel> levelModel);
    ~GameController();
    
    void setSnapshot(const SnapshotData& data);

    const std::shared_ptr<EntitiesController>& getEntitiesController() const { return m_entitiesController; }
    const std::shared_ptr<EntitiesModel>& getEntitiesModel() const { return m_entitiesModel; }
    const std::shared_ptr<LevelModel>& getLevelModel() const { return m_levelModel; }

    void setGameMode(const GameMode::Config& config, const bool host);
    const std::shared_ptr<GameMode>& getGameMode() const { return m_gameMode; }

    const float getCurrentTime() const { return m_currentTime; }

    bool isTileSolid(const cocos2d::Vec2& tilePos) const;
    
    void applyInput(const uint8_t playerID, std::shared_ptr<ClientInputMessage>& input);
    void applyInputs(const std::map<uint8_t, std::shared_ptr<ClientInputMessage>>& inputs);

    void tick(const float deltaTime);

    void spawnThings();

    std::shared_ptr<Entity> getEntityAtPoint(const cocos2d::Vec2& point,
                                             const uint32_t ignoreEntityID) const;

    void dumpEntities();

private:
    std::shared_ptr<EntitiesController> m_entitiesController;
    std::shared_ptr<EntitiesModel> m_entitiesModel;
    std::shared_ptr<LevelModel> m_levelModel;
    std::shared_ptr<GameMode> m_gameMode;
    float m_currentTime;
    bool m_host;
    
    const bool checkForReload(uint8_t playerID, const std::shared_ptr<ClientInputMessage>& input);
};

#endif /* GameController_h */
