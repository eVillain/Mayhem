#ifndef ClientController_h
#define ClientController_h

#include "cocos2d.h"
#include "cocos-ext.h"
#include "Network/NetworkMessages.h"
#include "Network/DrudgeNet/include/DataTypes.h"
#include "ClientModel.h"

class CameraController;
class CameraModel;
class ClientModel;
class Entity;
class LevelModel;
class GameView;
class GameViewController;
class HUDView;
class INetworkController;
class InputModel;
class LightController;
class Player;
class SnapshotModel;
class ReplayModel;
class GameSettings;
class GameModel;
class InputActionEvent;
class AppWillTerminateEvent;

namespace Net {
    class Message;
};

class ClientController
{
public:
    ClientController(std::shared_ptr<ClientModel> clientModel,
                     std::shared_ptr<GameSettings> gameSettings,
                     std::shared_ptr<GameViewController> gameViewController,
                     std::shared_ptr<LevelModel> levelModel,
                     std::shared_ptr<GameModel> gameModel,
                     std::shared_ptr<GameView> gameView,
                     std::shared_ptr<InputModel> inputModel,
                     std::shared_ptr<SnapshotModel> snapshotModel,
                     std::shared_ptr<ReplayModel> replayModel,
                     std::shared_ptr<INetworkController> networkController,
                     std::shared_ptr<LightController> lightController,
                     std::shared_ptr<HUDView> hudView);
    ~ClientController();

    void setMode(const ClientMode mode);
    void stop();
    
    void update(const float deltaTime);

    const SnapshotData& getDeltaData(const uint32_t serverTick);

private:
    void updateGame(const float deltaTime, const bool processInput);
    void predictLocalMovement(SnapshotData& snapshot);
    void updateEntities(const float alphaTime,
                        const SnapshotData& fromSnapshot,
                        const SnapshotData& toSnapshot,
                        const bool reachedNextSnapshot);
    
    void onDisconnected();
    void onNodeDisconnected(const Net::NodeID nodeID);
    void onStartGameReceived(const std::shared_ptr<Net::Message>& data, const Net::NodeID nodeID);
    void onSnapshotReceived(const std::shared_ptr<Net::Message>& data, const Net::NodeID nodeID);
    void onSnapshotDiffReceived(const std::shared_ptr<Net::Message>& data, const Net::NodeID nodeID);
    void onPlayerDeathReceived(const std::shared_ptr<Net::Message>& data, const Net::NodeID nodeID);
    void onTileDeathReceived(const std::shared_ptr<Net::Message>& data, const Net::NodeID nodeID);
    void onSpectateReceived(const std::shared_ptr<Net::Message>& data, const Net::NodeID nodeID);
    void onGameOverReceived(const std::shared_ptr<Net::Message>& data, const Net::NodeID nodeID);

    void onToggleClientPrediction();
    void onToggleInventory();
    void onInputAction(const InputActionEvent& event);

    void debugSnapshots(const size_t targetIndex, const float newAlpha);
    void checkShot(const SnapshotData& snapshot);
    void processIncomingSnapshot(const SnapshotData& snapshot);
    void processSnapshotHitData(const SnapshotData& snapshot);
    
    void onConfirmExitButton(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
    void onCancelExitButton(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
    void onSpectateButton(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
    void onRespawnButton(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);

    void onAppWillTerminate(const AppWillTerminateEvent&);

    std::shared_ptr<ClientInputMessage> getInputData() const;
    
    std::shared_ptr<ClientModel> m_clientModel;
    std::shared_ptr<GameSettings> m_gameSettings;
    std::shared_ptr<GameViewController> m_gameViewController;
    std::shared_ptr<LevelModel> m_levelModel;
    std::shared_ptr<GameModel> m_gameModel;
    std::shared_ptr<GameView> m_gameView;
    std::shared_ptr<InputModel> m_inputModel;
    std::shared_ptr<SnapshotModel> m_snapshotModel;
    std::shared_ptr<ReplayModel> m_replayModel;
    std::shared_ptr<INetworkController> m_networkController;
    std::shared_ptr<LightController> m_lightController;
    std::shared_ptr<HUDView> m_hudView;

    bool m_stopping;    
};

#endif /* ClientController_h */
