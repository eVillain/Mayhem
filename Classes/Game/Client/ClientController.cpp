#include "ClientController.h"

#include "CameraController.h"
#include "CameraModel.h"
#include "Utils/CollisionUtils.h"
#include "CrosshairView.h"
#include "Core/Dispatcher.h"
#include "EntityDataModel.h"
#include "FakeNet.h"
#include "FrameCache.h"
#include "GameView.h"
#include "GameViewController.h"
#include "HUDView.h"
#include "InitMainMenuCommand.h"
#include "Core/Injector.h"
#include "Game/Client/InputModel.h"
#include "LevelModel.h"
#include "LightController.h"
#include "MovementIntegrator.h"
#include "NetworkController.h"
#include "Network/NetworkMessages.h"
#include "PlayAudioEvent.h"
#include "Pseudo3DItem.h"
#include "Pseudo3DParticle.h"
#include "ServerController.h"
#include "SharedConstants.h"
#include "ShootToContinueLayer.h"
#include "SnapshotModel.h"
#include "SpawnParticlesEvent.h"
#include "ToggleClientPredictionEvent.h"
#include "ToggleInventoryEvent.h"
#include "WeaponConstants.h"
#include "PlayerLogic.h"
#include "ReplayModel.h"
#include "GameSettings.h"
#include "GameModel.h"

ClientController::ClientController(std::shared_ptr<ClientModel> clientModel,
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
                                   std::shared_ptr<HUDView> hudView)
: m_clientModel(clientModel)
, m_gameSettings(gameSettings)
, m_gameViewController(gameViewController)
, m_levelModel(levelModel)
, m_gameModel(gameModel)
, m_gameView(gameView)
, m_inputModel(inputModel)
, m_snapshotModel(snapshotModel)
, m_replayModel(replayModel)
, m_networkController(networkController)
, m_lightController(lightController)
, m_hudView(hudView)
, m_serverController(nullptr)
, m_stopping(false)
{
    Dispatcher::globalDispatcher().addListener(ToggleClientPredictionEvent::descriptor,
                                               std::bind(&ClientController::onToggleClientPredictionEvent, this, std::placeholders::_1));
}

ClientController::~ClientController()
{
    const cocos2d::Value& saveReplaySetting = m_gameSettings->getValue(ReplayModel::SETTING_SAVE_REPLAY, cocos2d::Value(true));
    if (saveReplaySetting.asBool())
    {
        m_replayModel->saveFile(ReplayModel::DEFAULT_REPLAY_FILE, m_gameModel->getTickRate());
    }
}

void ClientController::setMode(const ClientMode mode)
{
    m_clientModel->setMode(mode);
    m_networkController->addMessageCallback(MessageTypes::MESSAGE_TYPE_SERVER_SNAPSHOT,
                                            std::bind(&ClientController::onSnapshotReceived, this,
                                                      std::placeholders::_1, std::placeholders::_2));
    m_networkController->addMessageCallback(MessageTypes::MESSAGE_TYPE_SERVER_SNAPSHOT_DIFF,
                                            std::bind(&ClientController::onSnapshotDiffReceived, this,
                                                      std::placeholders::_1, std::placeholders::_2));
    m_networkController->addMessageCallback(MessageTypes::MESSAGE_TYPE_SERVER_STARTGAME,
                                            std::bind(&ClientController::onStartGameReceived, this,
                                                      std::placeholders::_1, std::placeholders::_2));
    m_networkController->addMessageCallback(MessageTypes::MESSAGE_TYPE_SERVER_PLAYER_DEATH,
                                            std::bind(&ClientController::onPlayerDeathReceived, this,
                                                      std::placeholders::_1, std::placeholders::_2));

    if (m_clientModel->getMode() == ClientMode::CLIENT_MODE_NETWORK)
    {
        m_networkController->setNodeDisconnectedCallback(std::bind(&ClientController::onNodeDisconnected, this,
                                                                   std::placeholders::_1));
        if (m_networkController->getTransport())
        {
            m_networkController->getTransport()->setDisconnectedCallback(std::bind(&ClientController::onDisconnected, this));
        }
    }
    else
    {
        m_serverController = Injector::globalInjector().getInstance<ServerController>();
        m_serverController->onNodeConnected(0);
        m_clientModel->setGameStarted(true);
        m_gameViewController->setCameraFollowPlayerID(0);
    }
    
    m_gameViewController->setRespawnCallback([this](){
        // Send respawn message to server
        std::shared_ptr<ClientStateUpdateMessage> respawnMessage = std::make_shared<ClientStateUpdateMessage>();
        respawnMessage->state = ClientState::PLAYER_RESPAWN;
        std::shared_ptr<Net::Message> message = respawnMessage;
        m_networkController->sendMessage(0, message, true);
    });
    
    // Send client level loaded message to server
    std::shared_ptr<ClientStateUpdateMessage> levelLoadedMessage = std::make_shared<ClientStateUpdateMessage>();
    levelLoadedMessage->state = ClientState::LEVEL_LOADED;
    std::shared_ptr<Net::Message> message = levelLoadedMessage;
    m_networkController->sendMessage(0, message, true);
}

void ClientController::stop()
{
    m_clientModel->setGameStarted(false);
    m_stopping = true;

    m_networkController->removeMessageCallback(MessageTypes::MESSAGE_TYPE_SERVER_SNAPSHOT);
    m_networkController->removeMessageCallback(MessageTypes::MESSAGE_TYPE_SERVER_STARTGAME);
    m_networkController->setNodeDisconnectedCallback(nullptr);
    m_networkController->getTransport()->setDisconnectedCallback(nullptr);
    InitMainMenuCommand initMainMenu;
    initMainMenu.run();
}

void ClientController::update(const float deltaTime)
{
    if (m_clientModel->getMode() == ClientMode::CLIENT_MODE_NETWORK)
    {
        m_networkController->receiveMessages();
        m_networkController->update(deltaTime);
        if (m_stopping)
        {
            return;
        }
    }
    
    float deltaAccumulator = m_gameModel->getDeltaAccumulator();

    if (m_clientModel->getGameStarted())
    {
        deltaAccumulator += deltaTime;
        m_gameModel->setCurrentTime(m_gameModel->getCurrentTime() + deltaTime);
    }

    bool processInput = false;
    if (deltaAccumulator >= m_gameModel->getFrameTime())
    {
        deltaAccumulator -= m_gameModel->getFrameTime();

        if (m_clientModel->getGameStarted())
        {
            m_gameModel->setCurrentTick(m_gameModel->getCurrentTick() + 1);

            std::shared_ptr<ClientInputMessage> currentInput = getInputData();
            std::shared_ptr<Net::Message> networkData = currentInput;

            // Save copy of inputs for local prediction
            m_clientModel->getInputData().push_back(currentInput);

            if (m_clientModel->getMode() == ClientMode::CLIENT_MODE_NETWORK)
            {
                m_networkController->sendMessage(0, networkData);
            }
            else
            {
                m_serverController->onInputMessageReceived(networkData, 0);
            }
            
            processInput = true;
        }
    }

    if (m_clientModel->getMode() == ClientMode::CLIENT_MODE_NETWORK)
    {
        m_networkController->sendMessages();
    }
    
    if (m_clientModel->getGameStarted())
    {
        m_gameModel->setDeltaAccumulator(deltaAccumulator);
        updateGame(deltaTime, processInput);
    }
}

const SnapshotData& ClientController::getDeltaData(const uint32_t serverTick)
{
    const auto& snapshots = m_snapshotModel->getSnapshots();
    auto snapshotIt = std::find_if(snapshots.begin(),
                                   snapshots.end(),
                                   [serverTick](const SnapshotData& data){
        return data.serverTick == serverTick;
    });
    if (snapshotIt == snapshots.end())
    {
        CCLOG("ClientController::getDeltaData fail");
        return SNAPSHOT_ZERO;
    }
    return *snapshotIt;
}

void ClientController::updateGame(const float deltaTime, const bool processInput)
{
    m_gameView->getDebugDrawNode()->clear();

    const float currentTickStartTime = m_gameModel->getCurrentTick() * m_gameModel->getFrameTime();
    float targetTime = currentTickStartTime + m_gameModel->getDeltaAccumulator() - m_clientModel->getInterpolationLatency();
    if (targetTime < 0.f)
    {
        targetTime = 0.f;
    }
    
    // Find snapshots in queue for target time
    const uint32_t targetFrame = std::floor(targetTime * m_gameModel->getTickRate());
    const size_t targetTimeSnapshotIndex = m_snapshotModel->getSnapshotIndexForFrame(targetFrame);
    if (targetTimeSnapshotIndex == 0)
    {
        return;
    }
    
    if (targetTimeSnapshotIndex > 1)
    {
        // Erase old snapshots
        m_snapshotModel->eraseUpToIndex(targetTimeSnapshotIndex - 1);
    }
    
    auto& snapshots = m_snapshotModel->getSnapshots();
    if (snapshots.size() < 2)
    {
        return;
    }
    
    const SnapshotData& fromSnapshot = snapshots[0];
    SnapshotData& toSnapshot = snapshots[1];
    
    // New snapshot reached
    const bool reachedNextSnapshot = toSnapshot.serverTick > m_snapshotModel->getLastApplied();
    if (reachedNextSnapshot)
    {
        if (m_clientModel->getPredictMovement())
        {
            predictLocalMovement(toSnapshot, fromSnapshot);
        }
        
        if (processInput && m_clientModel->getPredictBullets())
        {
            checkShot(toSnapshot);
        }
        
        m_snapshotModel->setLastApplied(toSnapshot.serverTick);
    }
        
    const float alphaTime = std::min(std::max(m_gameModel->getDeltaAccumulator() / m_gameModel->getFrameTime(), 0.f), 1.f);
    m_gameViewController->update(deltaTime,
                                 alphaTime,
                                 fromSnapshot,
                                 toSnapshot,
                                 reachedNextSnapshot,
                                 m_clientModel->getPredictBullets());

//    if (m_gameView->getDebugDrawNode()->isVisible())
//    {
//        m_gameView->addDebugLabel("INTERP", "Clienttime: " + std::to_string(m_clientModel->getCurrentTime()) +
//                                  " Target: " + std::to_string(targetTime)+
//                                  ") Alpha: " + std::to_string(alphaTime) +
//                                  " Tick: " + std::to_string(m_clientModel->getCurrentTick()) +
//                                  " From: " + std::to_string(fromSnapshot.serverTick) + " To: " + std::to_string(toSnapshot.serverTick));
//        
//        if (Injector::globalInjector().hasMapping<ServerController>())
//        {
//            const auto& serverController = Injector::globalInjector().getInstance<ServerController>();
//            m_gameView->addDebugLabel("SERVER", serverController->getDebugInfo());
//        }
//        
//        debugSnapshots(1, alphaTime);
//    }
}

void ClientController::predictLocalMovement(SnapshotData& toSnapshot,
                                            const SnapshotData& fromSnapshot)
{
    // Simulate forward inputs which were applied locally since server tick
    const auto playerIt = toSnapshot.playerData.find(m_clientModel->getLocalPlayerID());
    if (playerIt == toSnapshot.playerData.end())
    {
        return;
    }
    
    const PlayerState& playerState = playerIt->second;

    const uint16_t localPlayerEntityID = playerState.entityID;
    auto toEntityIt = toSnapshot.entityData.find(localPlayerEntityID);
    if (toEntityIt == toSnapshot.entityData.end())
    {
        return;
    }
    
    int removedInputs = 0;
    // Remove all inputs already processed on server at that snapshot
    while (!m_clientModel->getInputData().empty() &&
           m_clientModel->getInputData().begin()->get()->inputSequence <= toSnapshot.lastReceivedInput)
    {
        removedInputs++;
        m_clientModel->getInputData().erase(m_clientModel->getInputData().begin());
    }

    // Copy of current player entity state on server after inputs are applied
    EntitySnapshot toEntitySnapshot = toSnapshot.entityData.at(playerState.entityID);

    // Replay all remaining inputs on local player
    size_t index = 0;
    for (auto input : m_clientModel->getInputData())
    {
        float lastActionTime = m_clientModel->getLastPlayerActionTime();
        PlayerLogic::applyInput(m_clientModel->getLocalPlayerID(),
                                toSnapshot,
                                input,
                                lastActionTime,
                                m_gameModel->getCurrentTime());
        toEntitySnapshot = toSnapshot.entityData.at(playerState.entityID);
        m_clientModel->setLastPlayerActionTime(lastActionTime);
        
        MovementIntegrator::integratePosition(m_gameModel->getFrameTime(),
                                              localPlayerEntityID,
                                              toEntitySnapshot,
                                              toSnapshot.entityData,
                                              m_levelModel->getStaticRects());
        
        toSnapshot.entityData.at(playerState.entityID) = toEntitySnapshot;
        index++;
    }

    if (m_gameView->getDebugDrawNode()->isVisible())
    {
        const auto& fromEntityData = fromSnapshot.entityData.at(localPlayerEntityID);
        m_gameView->addDebugLabel("SNAPSHOTS", "Current pos: " + std::to_string(fromEntityData.positionX) + ", "  + std::to_string(toEntitySnapshot.positionY) +
                                  "From: " + std::to_string(fromEntityData.positionX) + ", "  + std::to_string(fromEntityData.positionY) +
                                  "To: " + std::to_string(toEntityIt->second.positionX) + ", "  + std::to_string(toEntityIt->second.positionY));


        m_gameView->addDebugLabel("INPUTS", "Predicted pos: " + std::to_string(toEntitySnapshot.positionX) + ", "  + std::to_string(toEntitySnapshot.positionY) +
                              " Inputs ahead: " + std::to_string(index) +
                              " Last received: " + std::to_string(toSnapshot.lastReceivedInput) +
                              " Unprocessed: " + std::to_string(m_clientModel->getInputData().size()) +
                              " Cleared: " + std::to_string(removedInputs));
    }
}

void ClientController::onDisconnected()
{
    stop();
}

void ClientController::onNodeDisconnected(const Net::NodeID nodeID)
{
    stop();
}

void ClientController::onStartGameReceived(const std::shared_ptr<Net::Message>& data, const Net::NodeID nodeID)
{
    if (auto startGameMessage = std::dynamic_pointer_cast<ServerStartGameMessage>(data))
    {
        m_clientModel->setLocalPlayerID(startGameMessage->playerID);
        m_gameViewController->setCameraFollowPlayerID(startGameMessage->playerID);
        
        m_clientModel->setGameStarted(true);
        m_gameModel->setDeltaAccumulator(0.f);
        auto glview = cocos2d::Director::getInstance()->getOpenGLView();
        glview->setViewName("Mayhem Royale - Player: " + std::to_string(startGameMessage->playerID));
    }
    else
    {
        CCLOG("ClientController::onStartGameReceived fail");
    }
}

void ClientController::onSnapshotReceived(const std::shared_ptr<Net::Message>& data, const Net::NodeID nodeID)
{
    if (auto snapshotMessage = std::dynamic_pointer_cast<ServerSnapshotMessage>(data))
    {
        m_snapshotModel->storeSnapshot(snapshotMessage->data);
        
        const cocos2d::Value& saveReplaySetting = m_gameSettings->getValue(ReplayModel::SETTING_SAVE_REPLAY, cocos2d::Value(true));
        if (saveReplaySetting.asBool())
        {
            m_replayModel->storeSnapshot(snapshotMessage->data);
        }
    }
    else
    {
        CCLOG("ClientController::onSnapshotReceived fail");
    }
}

void ClientController::onSnapshotDiffReceived(const std::shared_ptr<Net::Message>& data, const Net::NodeID nodeID)
{
    CCLOG("ClientController::onSnapshotDiffReceived not implemented!");
}

void ClientController::onToggleClientPredictionEvent(const Event& event)
{
    m_clientModel->setPredictBullets(!m_clientModel->getPredictBullets());
    m_clientModel->setPredictMovement(!m_clientModel->getPredictMovement());
    m_clientModel->setPredictAnimation(!m_clientModel->getPredictAnimation());
    
    if (m_gameView->getDebugDrawNode()->isVisible())
    {
        m_gameView->addDebugLabel("PREDICTION", "Prediction - Movement: " + std::to_string(m_clientModel->getPredictMovement()) +
                                  " Shooting: " + std::to_string(m_clientModel->getPredictBullets()) +
                                  " Animation: " + std::to_string(m_clientModel->getPredictAnimation()));
    }
}

void ClientController::onPlayerDeathReceived(const std::shared_ptr<Net::Message>& data, const Net::NodeID nodeID)
{
    if (auto deathMessage = std::dynamic_pointer_cast<ServerPlayerDeathMessage>(data))
    {
        CCLOG("ClientController::onPlayerDeathReceived %i killed by %i with %i",
              deathMessage->deadPlayerID, deathMessage->killerEntityID, deathMessage->killerType);
        
        std::string deadPlayerName = "DEAD_PLAYER";
        std::string killerPlayerName = "KILLER_PLAYER";
        const auto& names = m_clientModel->getPlayerNames();
        if (names.find(deathMessage->deadPlayerID) != names.end())
        {
            deadPlayerName = names.at(deathMessage->deadPlayerID);
        }
        m_hudView->getKillFeed()->onPlayerKilled(deadPlayerName,
                                                 killerPlayerName,
                                                 deathMessage->killerType,
                                                 deathMessage->headshot);
    }
    else
    {
        CCLOG("ClientController::onPlayerDeathReceived fail");
    }

}

void ClientController::debugSnapshots(const size_t targetIndex, const float newAlpha)
{
    const float xOffset = 100.f;
    const float spacing = 50.f;
    // Debug render snapshot playback
    for (size_t i = 0; i < m_snapshotModel->getSnapshots().size(); i++)
    {
        const cocos2d::Vec2 outerRectPos = cocos2d::Vec2(xOffset + (i * spacing), 20.f);
        const cocos2d::Vec2 innerRectPos = cocos2d::Vec2(xOffset + 1.f + (i * spacing), 21.f);
        m_gameView->getDebugDrawNode()->drawRect(outerRectPos, outerRectPos + cocos2d::Vec2(10.f, 20.f), cocos2d::Color4F::GRAY);
        if (i == targetIndex - 1)
        {
            m_gameView->getDebugDrawNode()->drawSolidRect(innerRectPos, innerRectPos + cocos2d::Vec2(9.f, 9.f), cocos2d::Color4F::GREEN);
        }
        else if (i == targetIndex)
        {
            m_gameView->getDebugDrawNode()->drawSolidRect(innerRectPos, innerRectPos + cocos2d::Vec2(9.f, 9.f), cocos2d::Color4F::YELLOW);
        }
        else if (i < targetIndex - 1)
        {
            m_gameView->getDebugDrawNode()->drawSolidRect(innerRectPos, innerRectPos + cocos2d::Vec2(9.f, 9.f), cocos2d::Color4F::RED);
        }
    }
    const float markerPos = xOffset + ((targetIndex - 1) * spacing) + (newAlpha * spacing);
    m_gameView->getDebugDrawNode()->drawLine(cocos2d::Vec2(markerPos, 5.f), cocos2d::Vec2(markerPos, 35.f), cocos2d::Color4F::BLUE);
}

void ClientController::checkShot(const SnapshotData& snapshot)
{
    // Check for predicted local player effects
    std::shared_ptr<ClientInputMessage>& currentInput = m_clientModel->getInputData().back();
    if (!currentInput->shoot)
    {
        return;
    }
    
    const auto playerIt = snapshot.playerData.find(m_clientModel->getLocalPlayerID());
    if (playerIt == snapshot.playerData.end())
    {
        return;
    }
    const PlayerState& player = playerIt->second;
    const uint32_t playerEntityID = player.entityID;
    
    const auto entityIt = snapshot.entityData.find(playerEntityID);
    if (entityIt == snapshot.entityData.end())
    {
        return;
    }
    const EntitySnapshot& entity = entityIt->second;
    
    if (m_clientModel->getInputData().empty())
    {
        return;
    }

    const InventoryItemState& weapon = player.weaponSlots.at(player.activeWeaponSlot);
    if (weapon.type != EntityType::PlayerEntity &&
        player.health > 0.f)
    {
        auto itemData = EntityDataModel::getStaticEntityData((EntityType)weapon.type);
        float shotDelay = itemData.weapon.timeShot;
        const size_t ammo = weapon.amount;
        float lastActionTime = m_clientModel->getLastPlayerActionTime();
        const bool canFire = (lastActionTime < 0.f ||
                              lastActionTime + shotDelay < m_gameModel->getCurrentTime());

        if (canFire && ammo > 0)
        {
            m_clientModel->setLastPlayerActionTime(m_gameModel->getCurrentTime());
            m_gameViewController->renderShot(player.entityID,
                                             playerIt->first,
                                             cocos2d::Vec2(entity.positionX, entity.positionY),
                                             player.flipX,
                                             cocos2d::Vec2(currentInput->aimPointX, currentInput->aimPointY),
                                             (EntityType)weapon.type,
                                             snapshot);
        }
    }
}

std::shared_ptr<ClientInputMessage> ClientController::getInputData() const
{
    const auto& snapshots = m_snapshotModel->getSnapshots();
    const cocos2d::Vec2 viewPosition = m_gameViewController->getCameraController()->getViewPosition();

    std::shared_ptr<ClientInputMessage> data = std::make_shared<ClientInputMessage>();
    data->inputSequence = m_gameModel->getCurrentTick();
    data->lastReceivedSnapshot = snapshots.size() ? snapshots.back().serverTick : 0;
    data->directionX = m_inputModel->getDirection().x;
    data->directionY = m_inputModel->getDirection().y;
    const cocos2d::Vec2 aimPoint = m_gameViewController->getAimPosition(m_inputModel->getMouseCoord());
    data->aimPointX = aimPoint.x;
    data->aimPointY = aimPoint.y;
    
    bool blockInput = m_hudView->getViewLayer() != nullptr;
    data->shoot = blockInput ? false : m_inputModel->getShoot();
    data->interact = blockInput ? false : m_inputModel->getInteract();
    data->run = m_inputModel->getRun();
    data->reload = m_inputModel->getReload();
    data->changeWeapon = m_inputModel->getChangeWeapon();
    data->slot = m_inputModel->getSlot();
    
    data->pickUpAmount = m_inputModel->getPickupAmount();
    data->pickUpType = m_inputModel->getPickupType();
    data->pickUpID = m_inputModel->getPickupID();

    if (data->pickUpType != 0) // Hack: 
    {
        data->interact = true;
    }
    
    m_inputModel->setPickupID(0);
    m_inputModel->setPickupAmount(0);
    m_inputModel->setPickupType(0);
    m_inputModel->setChangeWeapon(false);
    
    return data;
}

float ClientController::getTimeAlpha(const float targetTime,
                                     const uint32_t fromTick,
                                     const uint32_t toTick) const
{
    const float fromTime = fromTick * m_gameModel->getFrameTime();
    const float toTime = toTick * m_gameModel->getFrameTime();
    const float alpha = ((targetTime - fromTime) / (toTime - fromTime));
    return alpha;
}
