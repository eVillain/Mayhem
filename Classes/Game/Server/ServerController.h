#ifndef ServerController_h
#define ServerController_h

#include "Network/NetworkMessages.h"
#include "RaycastUtil.h"
#include "MovementIntegrator.h"
#include "cocos2d.h"

class BaseAI;
class GameController;
class LevelModel;
class FrameCache;
class InputCache;
class Entity;
class INetworkController;
class Player;
class Item;
class Projectile;
class GameModel;

namespace Net {
    class Message;
};

class ServerController
{
public:
    ServerController(std::shared_ptr<GameController> gameController,
                     std::shared_ptr<LevelModel> levelModel,
                     std::shared_ptr<GameModel> gameModel,
                     std::shared_ptr<INetworkController> networkController,
                     std::shared_ptr<FrameCache> frameCache,
                     std::shared_ptr<InputCache> inputCache);
    ~ServerController();

    void update(const float deltaTime);
    void stop();

    std::shared_ptr<GameController> getGameController() const { return m_gameController; }
    float getMaxPingThreshold() const { return m_maxPingThreshold; }

    const std::string getDebugInfo() const;
    
    void initDebugStuff();

private:
    enum ClientPlayerState {
        DISCONNECTED = 0,
        JOINING,
        CONNECTED,
    };
    
    std::shared_ptr<GameController> m_gameController;
    std::shared_ptr<LevelModel> m_levelModel;
    std::shared_ptr<GameModel> m_gameModel;
    std::shared_ptr<INetworkController> m_networkController;
    std::shared_ptr<FrameCache> m_frameCache;
    std::shared_ptr<InputCache> m_inputCache;
    
    float m_maxPingThreshold;
    bool m_sendDeltaUpdates;
    float m_gameOverTimer;

    std::map<uint32_t, EntitySnapshot> m_preRollbackState;
    std::map<uint8_t, ClientPlayerState> m_clientStates;
    std::map<uint8_t, std::vector<SnapshotData>> m_clientSnapshots;
    std::vector<FrameHitData> m_frameHitData;
    std::map<uint8_t, std::shared_ptr<BaseAI>> m_botPlayers;

    void performGameUpdate(const float deltaTime);
    void checkForShots(uint8_t playerID, const std::shared_ptr<ClientInputMessage>& input);
    const RayElement fireHitScanWeapon(const cocos2d::Vec2& shotStart,
                                       const cocos2d::Vec2& shotEnd,
                                       const uint16_t entityID,
                                       const EntityType weaponType,
                                       const int damage);
    void checkForInteraction(uint8_t playerID, const std::shared_ptr<ClientInputMessage>& input);
    void removeExpiredEntities();
    
    void onEntityCollision(const CollisionData& collisionData);
    bool onItemPickedUp(Player* player, Item* item);
    bool onProjectileHit(const std::shared_ptr<Entity>& entity,
                         const std::shared_ptr<Projectile>& projectile,
                         const size_t hitShapeIndex);
    void onProjectileDestroyed(const std::shared_ptr<Projectile>& projectile);
    void onTileDeath(const int tileX, const int tileY);
    void onGameModeWinConditionReached(uint8_t winner);
    
    void applyDamage(const std::shared_ptr<Player>& player,
                     const float damage,
                     const cocos2d::Vec2 position,
                     const uint16_t damagerID,
                     const EntityType damageType,
                     const size_t hitShapeIndex);
    
    bool wasWeaponFired(uint8_t playerID);
    void rollbackForPlayer(const uint8_t playerID, const uint32_t lastReceivedSnapshot);
    
    void onDisconnected();
    void onNodeConnected(const Net::NodeID nodeID);
    void onNodeDisconnected(const Net::NodeID nodeID);
    void onPlayerJoined(const uint8_t playerID);

    void onClientStateMessageReceived(const std::shared_ptr<Net::Message>& data,
                                      const Net::NodeID playerID);
    void onInputMessageReceived(const std::shared_ptr<Net::Message>& data,
                                const Net::NodeID playerID);

    std::shared_ptr<ServerSnapshotMessage> getFullWorldState(const Net::NodeID playerID);
    std::shared_ptr<ServerSnapshotMessage> getWorldStateDiff(const Net::NodeID playerID);

    void applyAI();
    
    cocos2d::Vec2 getAverageAICenter() const;
    cocos2d::Vec2 getAverageAIVelocity() const;
    cocos2d::Vec2 getAverageAwayVector(const cocos2d::Vec2& position, float distance) const;
    void integratePositions(const float deltaTime,
                            std::map<uint32_t, EntitySnapshot>& snapshot,
                            const std::vector<cocos2d::Rect>& staticRects);
    void sendUpdateMessages();
};

#endif /* ServerController_h */
