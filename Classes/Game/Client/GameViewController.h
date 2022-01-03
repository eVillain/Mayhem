#ifndef GameViewController_h
#define GameViewController_h

#include "cocos2d.h"
#include "Network/DrudgeNet/include/DataTypes.h"
#include "Network/NetworkMessages.h"
#include "Game/Shared/WeaponConstants.h"

class AudioController;
class CameraController;
class CameraModel;
class EntityView;
class LevelModel;
class GameSettings;
class GameView;
class HUDView;
class InputModel;
class LightController;
class Player;
class ParticlesController;

class GameViewController
{
public:
    GameViewController(std::shared_ptr<GameSettings> gameSettings,
                       std::shared_ptr<LevelModel> levelModel,
                       std::shared_ptr<GameView> gameView,
                       std::shared_ptr<InputModel> inputModel,
                       std::shared_ptr<LightController> lightController,
                       std::shared_ptr<ParticlesController> particlesController,
                       std::shared_ptr<HUDView> hudView,
                       std::shared_ptr<AudioController> audioController);
    ~GameViewController();
    
    void update(const float deltaTime,
                const SnapshotData& snapshot,
                const bool isNewSnapshot,
                const bool skipLocalPlayerShots);
    
    void setCameraFollowPlayerID(const uint8_t playerID);
    void setCameraFree();

    void onEntityDestroyed(const uint32_t entityID,
                           const SnapshotData& snapshot);
    void onEntitySpawned(const uint32_t entityID,
                         const EntitySnapshot& snapshot);
    
    void renderShot(const uint32_t shooterEntityID,
                    const uint8_t shooterPlayerID,
                    const cocos2d::Vec2 shooterPosition,
                    const bool shooterFlipX,
                    const cocos2d::Vec2 aimPoint,
                    const EntityType weaponType,
                    const SnapshotData& snapshot);

    void renderHitEffects(const cocos2d::Vec2& shotStart,
                          const cocos2d::Vec2& shotEnd,
                          const EntityType weaponType,
                          const WeaponConstants::WeaponStateData& data,
                          const uint16_t entityID,
                          const std::map<uint32_t, EntitySnapshot>& entityData);
    
    void renderHitData(const SnapshotData& snapshot, const bool skipLocalPlayerShots);
    
    cocos2d::Vec2 getWorldPosition(const cocos2d::Vec2& screenCoord) const;
    cocos2d::Vec2 getScreenPosition(const cocos2d::Vec2& worldCoord) const;
    const std::shared_ptr<CameraController>& getCameraController() const { return m_cameraController; }
    const std::shared_ptr<CameraModel>& getCameraModel() const { return m_cameraModel; }
    
    void setShotHitLastFrame(const bool hit) { m_shotHitLastFrame = hit; }

private:
    void updateCamera(const float deltaTime,
                      const SnapshotData& snapshot);
    
    void updateView(const float deltaTime);
    void updatePseudo3D(const float deltaTime);
    
    void updatePlayerAnimations(const uint8_t playerID,
                                std::shared_ptr<EntityView>& entityView,
                                const EntitySnapshot& entitySnapshot,
                                const PlayerState& playerState);
    
    void updateHeldItem(std::shared_ptr<EntityView>& entityView,
                        const PlayerState& state);
    
    void updateEntities(const SnapshotData& snapshot);
    
    void updateCursor(const SnapshotData& snapshot);
        
    void updateShotTrails(const float deltaTime);

    void updatePostProcess(const float zoomRadius);
    void renderPostProcess(const SnapshotData& snapshot);
    
    void renderDebug();
    
    void renderPlayerDeath(const cocos2d::Vec2& position,
                           const cocos2d::Vec2& direction,
                           const bool headShot);
    void onWindowResized(cocos2d::EventCustom*);
    
    std::shared_ptr<GameSettings> m_gameSettings;
    std::shared_ptr<LevelModel> m_levelModel;
    std::shared_ptr<CameraController> m_cameraController;
    std::shared_ptr<CameraModel> m_cameraModel;
    std::shared_ptr<GameView> m_gameView;
    std::shared_ptr<InputModel> m_inputModel;
    std::shared_ptr<LightController> m_lightController;
    std::shared_ptr<ParticlesController> m_particlesController;
    std::shared_ptr<HUDView> m_hudView;
    std::shared_ptr<AudioController> m_audioController;
    cocos2d::RefPtr<cocos2d::GLProgram> m_postProcessShader;
    cocos2d::RefPtr<cocos2d::GLProgram> m_postProcessNoLightShader;

    cocos2d::EventListenerCustom* m_windowResizeListener;
    bool m_shotHitLastFrame;
    
    std::map<uint32_t, std::shared_ptr<EntityView>> m_entityViews;
    
    struct ShotTrail
    {
        float time;
        cocos2d::Vec2 start;
        cocos2d::Vec2 end;
    };
    std::vector<ShotTrail> m_shotTrails;
};

#endif /* GameViewController_h */
