#include "ClientController.h"

#include "CameraController.h"
#include "CameraModel.h"
#include "Core/Dispatcher.h"
#include "Core/Injector.h"
#include "CrosshairView.h"
#include "EntityDataModel.h"
#include "Game/Client/InputModel.h"
#include "GameModel.h"
#include "GameSettings.h"
#include "GameView.h"
#include "GameViewController.h"
#include "HUDView.h"
#include "LevelModel.h"
#include "LightController.h"
#include "MovementIntegrator.h"
#include "Network/NetworkMessages.h"
#include "NetworkController.h"
#include "PlayerLogic.h"
#include "Pseudo3DItem.h"
#include "Pseudo3DParticle.h"
#include "ReplayModel.h"
#include "SharedConstants.h"
#include "SnapshotModel.h"
#include "Utils/CollisionUtils.h"
#include "WeaponConstants.h"
#include "InputConstants.h"

#include "ShutdownClientCommand.h"
#include "ShutdownLocalServerCommand.h"

#include "InventoryLayer.h"
#include "ConfirmToContinueLayer.h"
#include "ExitGameLayer.h"
#include "GameOverLayer.h"

#include "ToggleInventoryEvent.h"
#include "BackButtonPressedEvent.h"
#include "SpawnParticlesEvent.h"
#include "ToggleClientPredictionEvent.h"

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
, m_stopping(false)
{
    Dispatcher::globalDispatcher().addListener(ToggleClientPredictionEvent::descriptor,
                                               std::bind(&ClientController::onToggleClientPredictionEvent, this, std::placeholders::_1));
    Dispatcher::globalDispatcher().addListener(ToggleInventoryEvent::descriptor,
                                               std::bind(&ClientController::onToggleInventoryEvent, this, std::placeholders::_1));
    Dispatcher::globalDispatcher().addListener(BackButtonPressedEvent::descriptor, std::bind(&ClientController::onBackButtonPressed, this, std::placeholders::_1));
    printf("ClientController:: constructor: %p\n", this);
}

ClientController::~ClientController()
{
    printf("ClientController:: destructor: %p\n", this);
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
    m_networkController->addMessageCallback(MessageTypes::MESSAGE_TYPE_SERVER_TILE_DEATH,
                                            std::bind(&ClientController::onTileDeathReceived, this,
                                                      std::placeholders::_1, std::placeholders::_2));
    m_networkController->addMessageCallback(MessageTypes::MESSAGE_TYPE_SERVER_SPECTATE,
                                            std::bind(&ClientController::onSpectateReceived, this,
                                                      std::placeholders::_1, std::placeholders::_2));
    m_networkController->addMessageCallback(MessageTypes::MESSAGE_TYPE_SERVER_GAME_OVER,
                                            std::bind(&ClientController::onGameOverReceived, this,
                                                      std::placeholders::_1, std::placeholders::_2));

    m_networkController->setNodeDisconnectedCallback(std::bind(&ClientController::onNodeDisconnected, this,
                                                               std::placeholders::_1));

    if (m_networkController->getTransport())
    {
        m_networkController->getTransport()->setDisconnectedCallback(std::bind(&ClientController::onDisconnected, this));
    }

    m_stopping = false;

    // Send client level loaded message to server
    std::shared_ptr<ClientStateUpdateMessage> levelLoadedMessage = std::make_shared<ClientStateUpdateMessage>();
    levelLoadedMessage->state = ClientState::LEVEL_LOADED;
    std::shared_ptr<Net::Message> message = levelLoadedMessage;
    m_networkController->sendMessage(0, message, true);
    
    if (m_clientModel->getMode() == ClientMode::CLIENT_MODE_LOCAL)
    {
        // beautiful hack to prevent local client from running ahead of local server
        m_gameModel->setCurrentTime(-m_gameModel->getFrameTime());
        m_gameModel->setCurrentTick(-1);
    }
    
    m_hudView->setTeamsVisible(m_gameModel->getPlayersPerTeam() > 1);
    m_hudView->setSpectatorsVisible(false);
    m_hudView->setKills(0);
    m_hudView->setPlayersAlive(0);

    const cocos2d::Value& playerNameSetting = m_gameSettings->getValue(GameSettings::SETTING_PLAYER_NAME, cocos2d::Value(""));
    m_clientModel->setPlayerName(m_clientModel->getLocalPlayerID(), playerNameSetting.asString());
}

void ClientController::stop()
{
    m_clientModel->setGameStarted(false);
    m_stopping = true;

    const cocos2d::Value& saveReplaySetting = m_gameSettings->getValue(ReplayModel::SETTING_SAVE_REPLAY, cocos2d::Value(true));
    if (saveReplaySetting.asBool())
    {
        m_replayModel->saveFile(ReplayModel::DEFAULT_REPLAY_FILE, m_gameModel->getTickRate());
    }
    
    m_networkController->removeMessageCallback(MessageTypes::MESSAGE_TYPE_SERVER_SNAPSHOT);
    m_networkController->removeMessageCallback(MessageTypes::MESSAGE_TYPE_SERVER_SNAPSHOT_DIFF);
    m_networkController->removeMessageCallback(MessageTypes::MESSAGE_TYPE_SERVER_STARTGAME);
    m_networkController->removeMessageCallback(MessageTypes::MESSAGE_TYPE_SERVER_PLAYER_DEATH);
    m_networkController->removeMessageCallback(MessageTypes::MESSAGE_TYPE_SERVER_TILE_DEATH);
    m_networkController->removeMessageCallback(MessageTypes::MESSAGE_TYPE_SERVER_SPECTATE);
    m_networkController->removeMessageCallback(MessageTypes::MESSAGE_TYPE_SERVER_GAME_OVER);
    m_networkController->setNodeDisconnectedCallback(nullptr);
    
    if (m_networkController->getTransport())
    {
        m_networkController->getTransport()->setDisconnectedCallback(nullptr);
    }
    m_networkController->terminate();
    
    ShutdownLocalServerCommand shutdownServer;
    shutdownServer.run();
    
    Dispatcher::globalDispatcher().removeListeners(BackButtonPressedEvent::descriptor);
    Dispatcher::globalDispatcher().removeListeners(ToggleClientPredictionEvent::descriptor);
    Dispatcher::globalDispatcher().removeListeners(ToggleInventoryEvent::descriptor);
    
    ShutdownClientCommand shutdownClient;
    shutdownClient.run();
}

void ClientController::update(const float deltaTime)
{
    if (m_clientModel->getMode() == ClientMode::CLIENT_MODE_NETWORK)
    {
        m_networkController->receiveMessages();

        m_networkController->update(deltaTime);
    }
    
    if (m_stopping)
    {
        return;
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

            m_networkController->sendMessage(0, networkData);

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
        
        if (m_hudView->getViewLayer() &&
            m_hudView->getViewLayer()->getDescriptor() == InventoryLayer::DESCRIPTOR)
        {
            auto inventoryLayer = std::dynamic_pointer_cast<InventoryLayer>(m_hudView->getViewLayer());
            inventoryLayer->setData(m_clientModel->getLocalPlayerID());
        }
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
    auto& snapshots = m_snapshotModel->getSnapshots();
    if (snapshots.empty())
    {
        return;
    }
    const uint32_t ticksToBuffer = m_clientModel->getTicksToBuffer() + 2;
    const auto& latestSnapshot = snapshots.back();
    const uint32_t latestSnapshotTick = latestSnapshot.serverTick;
    if (latestSnapshotTick > m_gameModel->getCurrentTick() + ticksToBuffer)
    {
        printf("server is ahead - fast-forwarding from tick %u to %u\n", m_gameModel->getCurrentTick(), latestSnapshotTick);
        // Game server is ahead too much, we should fast-forward the client
        m_gameModel->setCurrentTick(latestSnapshotTick - ticksToBuffer);
    }
    
    const float currentTickStartTime = m_gameModel->getCurrentTick() * m_gameModel->getFrameTime();
    const float targetTime = std::max(currentTickStartTime + m_gameModel->getDeltaAccumulator(), 0.f);

    // Find snapshots in queue for target time
    const uint32_t targetFrame = std::floor(targetTime * m_gameModel->getTickRate());
    const size_t targetTimeSnapshotIndex = m_snapshotModel->getSnapshotIndexForFrame(targetFrame);
//    printf("target time: %f, target frame: %u, index: %lu\n", targetTime, targetFrame, targetTimeSnapshotIndex);

    if (targetTimeSnapshotIndex > 1)
    {
        // Erase old snapshots
        m_snapshotModel->eraseUpToIndex(targetTimeSnapshotIndex - 1);
    }

    const SnapshotData& fromSnapshot = snapshots[0];
    SnapshotData& toSnapshot = snapshots.size() > 1 ? snapshots[1] : snapshots[0];

    // New snapshot reached
    const bool reachedNextSnapshot = toSnapshot.serverTick > m_snapshotModel->getLastApplied();
    if (reachedNextSnapshot)
    {
        if (m_clientModel->getPredictMovement())
        {
            predictLocalMovement(toSnapshot);
        }
        m_hudView->update(toSnapshot, m_clientModel->getLocalPlayerID());

        m_snapshotModel->setLastApplied(toSnapshot.serverTick);
    }

    const float alphaTime = std::min(std::max(m_gameModel->getDeltaAccumulator() / m_gameModel->getFrameTime(), 0.f), 1.f);
    auto interpolatedSnapshot = SnapshotModel::interpolateSnapshots(fromSnapshot, toSnapshot, alphaTime);
    m_gameViewController->update(deltaTime,
                                 interpolatedSnapshot,
                                 reachedNextSnapshot,
                                 m_clientModel->getPredictBullets());
    
    if (reachedNextSnapshot &&
        processInput &&
        m_clientModel->getPredictBullets())
    {
        checkShot(toSnapshot);
    }
    
    if (m_gameView->getDebugDrawNode()->isVisible())
    {
        debugSnapshots(targetTimeSnapshotIndex, alphaTime);
    }
}

void ClientController::predictLocalMovement(SnapshotData& snapshot)
{
    // Simulate forward inputs which were applied locally since server tick
    const auto playerIt = snapshot.playerData.find(m_clientModel->getLocalPlayerID());
    if (playerIt == snapshot.playerData.end())
    {
        return; // Player is dead, nothing to predict here :D
    }
    
    const PlayerState& playerState = playerIt->second;

    const uint16_t localPlayerEntityID = playerState.entityID;
    auto toEntityIt = snapshot.entityData.find(localPlayerEntityID);
    if (toEntityIt == snapshot.entityData.end())
    {
        return; // No entity data for player, cant predict anything here
    }
    
    // Copy of current player entity state on server before local inputs are applied
    EntitySnapshot toEntitySnapshot = snapshot.entityData.at(playerState.entityID);
    
    int removedInputs = 0;
    // Remove all inputs already processed on server at that snapshot
    while (!m_clientModel->getInputData().empty() &&
           m_clientModel->getInputData().begin()->get()->inputSequence <= snapshot.lastReceivedInput)
    {
        removedInputs++;
        m_clientModel->getInputData().erase(m_clientModel->getInputData().begin());
    }
    
    if (m_gameView->getDebugDrawNode()->isVisible())
    {
        m_gameView->addDebugLabel("SNAPSHOTS",
                                  "Server pos: " + std::to_string(toEntitySnapshot.positionX) +
                                  ", "  + std::to_string(toEntitySnapshot.positionY) +
                                  " at tick: " + std::to_string(snapshot.serverTick) +
                                  " Last received: " + std::to_string(snapshot.lastReceivedInput));
    }

    // Replay all remaining inputs on local client
    size_t index = 0;
    for (auto input : m_clientModel->getInputData())
    {
        float lastActionTime = m_clientModel->getLastPlayerActionTime();
        PlayerLogic::applyInput(m_clientModel->getLocalPlayerID(),
                                snapshot,
                                input,
                                lastActionTime,
                                m_gameModel->getCurrentTime());
        toEntitySnapshot = snapshot.entityData.at(playerState.entityID);
        m_clientModel->setLastPlayerActionTime(lastActionTime);
        
        const cocos2d::Vec2 direction = cocos2d::Vec2(input->directionX, input->directionY);
        const cocos2d::Vec2 velocity = PlayerLogic::getMovementVelocityForInput(direction, input->run);
        const float angularVelocity = 0.f;
        MovementIntegrator::integratePosition(m_gameModel->getFrameTime(),
                                              localPlayerEntityID,
                                              toEntitySnapshot,
                                              velocity,
                                              angularVelocity,
                                              snapshot.entityData,
                                              m_levelModel->getStaticRects());
        
        snapshot.entityData.at(playerState.entityID) = toEntitySnapshot;
        index++;
    }

    if (m_gameView->getDebugDrawNode()->isVisible())
    {
        m_gameView->addDebugLabel("INPUTS",
                                  "Predicted pos: " + std::to_string(toEntitySnapshot.positionX) +
                                  ", "  + std::to_string(toEntitySnapshot.positionY) +
                                  " Inputs ahead: " + std::to_string(index) +
                                  " Removed: " + std::to_string(removedInputs));
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
        if (m_snapshotModel->storeSnapshot(snapshotMessage->data))
        {
            processIncomingSnapshot(snapshotMessage->data);
        }
        
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

void ClientController::onToggleInventoryEvent(const Event& event)
{
    if (!m_clientModel->getLocalPlayerAlive())
    {
        return;
    }
    
    if (m_hudView->getViewLayer())
    {
        if (m_hudView->getViewLayer()->getDescriptor() != InventoryLayer::DESCRIPTOR)
        {
            return;
        }
        m_hudView->removeViewLayer();
        return;
    }
    auto inventoryLayer = Injector::globalInjector().instantiateUnmapped<InventoryLayer,
        SnapshotModel>();
    inventoryLayer->initialize();
    inventoryLayer->setItemPickupCallback([this](const EntityType type,
                                                 const uint16_t amount,
                                                 const uint16_t entityID){
        m_inputModel->setInputValue(InputConstants::ACTION_INTERACT, 1.f);
        m_inputModel->setPickupType((uint8_t)type);
        m_inputModel->setPickupAmount(amount);
        m_inputModel->setPickupID(entityID);
    });
    m_hudView->setViewLayer(inventoryLayer);
    
    inventoryLayer->setData(m_clientModel->getLocalPlayerID());
}

void ClientController::onBackButtonPressed(const Event& event)
{
    if (m_hudView->getViewLayer())
    {
        if (m_hudView->getViewLayer()->getDescriptor() == InventoryLayer::DESCRIPTOR)
        {
            m_hudView->removeViewLayer();
        }
        return;
    }
    
    auto exitGameLayer = std::make_shared<ExitGameLayer>();
    exitGameLayer->setup("Are you sure you want to leave?",
                         "EXIT GAME");
    exitGameLayer->getCancelButton()->addTouchEventListener(CC_CALLBACK_2(ClientController::onCancelExitButton, this));
    exitGameLayer->getConfirmButton()->addTouchEventListener(CC_CALLBACK_2(ClientController::onConfirmExitButton, this));

    m_hudView->setViewLayer(exitGameLayer);
}

void ClientController::onPlayerDeathReceived(const std::shared_ptr<Net::Message>& data, const Net::NodeID nodeID)
{
    if (auto deathMessage = std::dynamic_pointer_cast<ServerPlayerDeathMessage>(data))
    {
        CCLOG("ClientController::onPlayerDeathReceived %i killed by %i with %i",
              deathMessage->deadPlayerID, deathMessage->killerEntityID, deathMessage->killerType);
        
        std::string deadPlayerName = m_clientModel->getPlayerName(deathMessage->deadPlayerID);
        std::string killerPlayerName = m_clientModel->getPlayerName(m_clientModel->getLocalPlayerID());
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

void ClientController::onTileDeathReceived(const std::shared_ptr<Net::Message>& data, const Net::NodeID nodeID)
{
    if (auto deathMessage = std::dynamic_pointer_cast<ServerTileDeathMessage>(data))
    {
//        CCLOG("ClientController::onTileDeathReceived at: %i, %i",
//              deathMessage->tileX, deathMessage->tileY);
        
        const cocos2d::Vec2 tile = cocos2d::Vec2(deathMessage->tileX, deathMessage->tileY);
        cocos2d::TMXLayer* foreground = m_gameView->getFGTilesNode();
        cocos2d::TMXLayer* background = m_gameView->getBGTilesNode();
        cocos2d::TMXLayer* staticLights = m_gameView->getStaticLightingNode();
        cocos2d::Sprite* foregroundSprite = foreground->getTileAt(tile);
        cocos2d::Sprite* backgroundSprite = background->getTileAt(tile);
        cocos2d::Sprite* lightSprite = staticLights->getTileAt(tile);

        if (foregroundSprite)
        {
            foregroundSprite->runAction(cocos2d::FadeOut::create(1.f));
        }
        if (backgroundSprite)
        {
            backgroundSprite->runAction(cocos2d::FadeOut::create(1.f));
        }
        if (lightSprite)
        {
            lightSprite->runAction(cocos2d::FadeOut::create(1.f));
        }
    }
    else
    {
        CCLOG("ClientController::onTileDeathReceived fail");
    }
}

void ClientController::onSpectateReceived(const std::shared_ptr<Net::Message>& data,
                                          const Net::NodeID nodeID)
{
    if (auto spectateMessage = std::dynamic_pointer_cast<ServerSpectateMessage>(data))
    {
        m_gameViewController->getCameraModel()->setCameraFollowPlayerID(spectateMessage->spectatingPlayerID);
        if (m_hudView->getViewLayer() &&
            m_hudView->getViewLayer()->getDescriptor() == ConfirmToContinueLayer::DESCRIPTOR)
        {
            m_hudView->removeViewLayer();
        }
    }
    else
    {
        CCLOG("ClientController::onSpectateReceived fail");
    }
}

void ClientController::onGameOverReceived(const std::shared_ptr<Net::Message>& data,
                                          const Net::NodeID nodeID)
{
    if (auto gameOverMessage = std::dynamic_pointer_cast<ServerGameOverMessage>(data))
    {
        static const std::string FIRST_PLACE_SOLO_TITLE = "You have emerged victorious!";
        static const std::string FIRST_PLACE_SOLO_TEXT = "Bask in thy glory!!!";
//        static const std::string FIRST_PLACE_TEAM_TITLE = "Your team has emerged victorious!";
//        static const std::string SECOND_PLACE_TITLE = "First Is The Worst, Second Is The Best...";
//        static const std::string SECOND_PLACE_TEXT = "Third Is The One With The Hairy Chest";
        static const std::string LOST_SOLO_TITLE = "You have lost.";
        static const std::string LOST_SOLO_TEXT = "Better luck next time...";

        // TODO: Handle multiple teams on client + get placement from server
        const bool isWinner = gameOverMessage->winnerID == m_clientModel->getLocalPlayerID();
        const std::string title = isWinner ? FIRST_PLACE_SOLO_TITLE : LOST_SOLO_TITLE;
        const std::string text = isWinner ? FIRST_PLACE_SOLO_TEXT : LOST_SOLO_TEXT;
        auto gameOverLayer = std::make_shared<GameOverLayer>();
        gameOverLayer->setup(title, text);
        gameOverLayer->getExitButton()->addTouchEventListener(CC_CALLBACK_2(ClientController::onConfirmExitButton, this));
        m_hudView->setViewLayer(gameOverLayer);
    }
    else
    {
        CCLOG("ClientController::onGameOverReceived fail");
    }
}

void ClientController::debugSnapshots(const size_t targetIndex, const float alphaTime)
{
    // Zoomed-in detail view of frames
     const float DETAIL_WIDTH = 400.f;
     const float DETAIL_HEIGHT = 30.f;
     const float DETAIL_POS_X = 500.f - DETAIL_WIDTH;
     const float DETAIL_POS_Y = 38.f;
     m_gameView->getDebugDrawNode()->drawRect(cocos2d::Vec2(DETAIL_POS_X,
                                                            DETAIL_POS_Y),
                                              cocos2d::Vec2(DETAIL_POS_X + DETAIL_WIDTH,
                                                            DETAIL_POS_Y + DETAIL_HEIGHT),
                                              cocos2d::Color4F::BLACK);
     
     // Detail view cursor
     m_gameView->getDebugDrawNode()->drawLine(cocos2d::Vec2(DETAIL_POS_X + DETAIL_WIDTH*0.5f, DETAIL_POS_Y),
                                              cocos2d::Vec2(DETAIL_POS_X + DETAIL_WIDTH*0.5f, DETAIL_POS_Y + DETAIL_HEIGHT),
                                              cocos2d::Color4F::GREEN);

     const uint32_t VISIBLE_FRAMES = 10;
     const uint32_t VISIBLE_RANGE = VISIBLE_FRAMES/2;

     const float FRAME_WIDTH = DETAIL_WIDTH / VISIBLE_FRAMES;
     const float FRAME_HEIGHT = DETAIL_HEIGHT - (8.f * 2);
     const uint32_t targetFrame = std::floor(m_gameModel->getCurrentTime() * m_gameModel->getTickRate());
     const uint32_t firstVisibleFrame = targetFrame >= VISIBLE_RANGE ? targetFrame - VISIBLE_RANGE : 0;
     const uint32_t lastVisibleFrame = targetFrame > m_snapshotModel->getSnapshots().size() - VISIBLE_RANGE ? (uint32_t)m_snapshotModel->getSnapshots().size() : targetFrame + VISIBLE_RANGE;

     const float offsetX = -alphaTime * FRAME_WIDTH;
     const float posY = DETAIL_POS_Y + 8.f;

    // Debug render snapshot playback
    for (const auto& snapshot : m_snapshotModel->getSnapshots())
    {
        if (snapshot.serverTick < firstVisibleFrame ||
            snapshot.serverTick > lastVisibleFrame)
        {
            continue;
        }

        const float posX = DETAIL_POS_X + offsetX + ((snapshot.serverTick - (targetFrame - VISIBLE_RANGE)) * FRAME_WIDTH);
        const float alpha = 1.f-fabs((float(snapshot.serverTick) - targetFrame) / VISIBLE_RANGE);
        
        cocos2d::Color4F color = (snapshot.serverTick % m_gameModel->getTickRate() == 0) ? cocos2d::Color4F::WHITE : cocos2d::Color4F::GRAY;
        color.a = alpha;
        m_gameView->getDebugDrawNode()->drawRect(cocos2d::Vec2(posX, posY),
                                                 cocos2d::Vec2(posX + FRAME_WIDTH - 1, posY + FRAME_HEIGHT),
                                                 color);
        
        if (!snapshot.hitData.empty())
        {
            m_gameView->getDebugDrawNode()->drawSolidRect(cocos2d::Vec2(posX+1, posY+1),
                                                          cocos2d::Vec2(posX + FRAME_WIDTH - 2, posY + FRAME_HEIGHT - 1),
                                                          cocos2d::Color4F(1.f, 0.f, 0.f, alpha));

        }
    }
}

void ClientController::checkShot(const SnapshotData& snapshot)
{
    // Check for predicted local player effects
    if (m_clientModel->getInputData().empty())
    {
        return;
    }
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
    if (weapon.type != EntityType::NoEntity &&
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
    data->lastReceivedSnapshot = snapshots.empty() ? 0 : snapshots.back().serverTick;
    data->directionX = m_inputModel->getInputValue(InputConstants::ACTION_MOVE_RIGHT);
    data->directionY = -m_inputModel->getInputValue(InputConstants::ACTION_MOVE_UP);
    const cocos2d::Vec2 aimPoint = m_gameViewController->getAimPosition(m_inputModel->getMouseCoord());
    data->aimPointX = aimPoint.x;
    data->aimPointY = aimPoint.y;
    
    bool blockInput = !m_clientModel->getLocalPlayerAlive() || m_hudView->getViewLayer();
    data->shoot = blockInput ? false : m_inputModel->getInputValue(InputConstants::ACTION_SHOOT);
    data->interact = blockInput ? false : m_inputModel->getInputValue(InputConstants::ACTION_INTERACT);
    data->run = m_inputModel->getInputValue(InputConstants::ACTION_RUN);
    data->reload = m_inputModel->getInputValue(InputConstants::ACTION_RELOAD);
    data->changeWeapon = m_inputModel->getChangeWeapon();
    data->slot = m_inputModel->getSlot();
    
    data->pickUpAmount = m_inputModel->getPickupAmount();
    data->pickUpType = m_inputModel->getPickupType();
    data->pickUpID = m_inputModel->getPickupID();

    if (data->pickUpType != 0) // Hack: makes it so we can grab stuff in inventory
    {
        data->interact = true;
    }
    
    m_inputModel->setPickupID(0);
    m_inputModel->setPickupAmount(0);
    m_inputModel->setPickupType(0);
    m_inputModel->setChangeWeapon(false);
    
    return data;
}

void ClientController::processIncomingSnapshot(const SnapshotData& snapshot)
{
    const uint8_t localPlayerID = m_clientModel->getLocalPlayerID();
    bool wasLocalPlayerAlive = m_clientModel->getLocalPlayerAlive();
    bool isLocalPlayerAlive = false;
    
    const bool containsLocalPlayer = snapshot.playerData.find(localPlayerID) != snapshot.playerData.end();
    if (containsLocalPlayer)
    {
        const auto& playerData = snapshot.playerData.at(localPlayerID);
        isLocalPlayerAlive = playerData.health > 0.f;
    }

    if (wasLocalPlayerAlive && !isLocalPlayerAlive)
    {
        m_clientModel->setLocalPlayerAlive(false);

        if (m_hudView->getViewLayer() &&
            m_hudView->getViewLayer()->getDescriptor() == GameOverLayer::DESCRIPTOR)
        {
            return;
        }
        
        const bool allowRespawn = (m_gameModel->getGameModeType() != GameModeType::GAME_MODE_BATTLEROYALE);
        const std::string titleText = "YOU DIED";
        const std::string messageText = allowRespawn ? "Press button to respawn" : "Press button to spectate";
        const std::string buttonText = allowRespawn ? "Respawn" : "Spectate";
        auto confirmToContinueLayer = Injector::globalInjector().instantiateUnmapped<ConfirmToContinueLayer>();
        confirmToContinueLayer->setup(titleText,
                                      messageText,
                                      buttonText);
        if (allowRespawn)
        {
            confirmToContinueLayer->getConfirmButton()->addTouchEventListener(CC_CALLBACK_2(ClientController::onRespawnButton, this));
        }
        else
        {
            confirmToContinueLayer->getConfirmButton()->addTouchEventListener(CC_CALLBACK_2(ClientController::onSpectateButton, this));
        }
        m_hudView->setViewLayer(confirmToContinueLayer);
    }
    else if (!wasLocalPlayerAlive && isLocalPlayerAlive)
    {
        m_hudView->removeViewLayer();
        m_clientModel->setLocalPlayerAlive(true);
    }
    
    processSnapshotHitData(snapshot);
}

void ClientController::processSnapshotHitData(const SnapshotData& snapshot)
{
    bool shotHitLastFrame = false;
    const uint8_t localPlayerID = m_clientModel->getLocalPlayerID();
    const std::vector<FrameHitData>& hitData = snapshot.hitData;
    for (const auto& hit : hitData)
    {
        if (hit.damage == 0.f)
        {
            continue;
        }
        if (hit.hitEntityID == 0)
        {
            continue;
        }
        
        auto hitterPlayerIt = std::find_if(snapshot.playerData.begin(),
                                           snapshot.playerData.end(),
                                           [&hit](const std::pair<uint8_t, PlayerState>& pair) {
            return pair.second.entityID == hit.hitterEntityID;
        });
        
        if (hitterPlayerIt == snapshot.playerData.end())
        {
            continue;
        }
        
        if (hitterPlayerIt->first != localPlayerID)
        {
            continue;
        }
        
        // If confirmed a hit on someone show hit marker on cursor
        shotHitLastFrame = true;
        break;
    }

    m_gameViewController->setShotHitLastFrame(shotHitLastFrame);
}

void ClientController::onConfirmExitButton(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type)
{
    if (type != cocos2d::ui::Widget::TouchEventType::ENDED)
    {
        return;
    }
    stop();
}

void ClientController::onCancelExitButton(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type)
{
    if (type != cocos2d::ui::Widget::TouchEventType::ENDED)
    {
        return;
    }
    m_hudView->removeViewLayer();
}

void ClientController::onSpectateButton(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type)
{
    std::shared_ptr<ClientStateUpdateMessage> updateMessage = std::make_shared<ClientStateUpdateMessage>();
    updateMessage->state = ClientState::PLAYER_SPECTATE;
    std::shared_ptr<Net::Message> message = updateMessage;
    m_networkController->sendMessage(0, message, true);
}

void ClientController::onRespawnButton(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type)
{
    if (m_hudView->getViewLayer() &&
        m_hudView->getViewLayer()->getDescriptor() == ConfirmToContinueLayer::DESCRIPTOR)
    {
        auto layer = std::dynamic_pointer_cast<ConfirmToContinueLayer>(m_hudView->getViewLayer());
        cocos2d::FadeOut* fadeOut = cocos2d::FadeOut::create(0.5f);
        cocos2d::CallFunc* changeTextCB = cocos2d::CallFunc::create([layer](){
            layer->getContinueLabel()->setString("Waiting to respawn...");
        });
        cocos2d::FadeIn* fadeIn = cocos2d::FadeIn::create(0.5f);
        cocos2d::Sequence* sequence = cocos2d::Sequence::create(fadeOut, changeTextCB, fadeIn, NULL);
        layer->getContinueLabel()->runAction(sequence);
    }
    
    // Send respawn message to server
    std::shared_ptr<ClientStateUpdateMessage> respawnMessage = std::make_shared<ClientStateUpdateMessage>();
    respawnMessage->state = ClientState::PLAYER_RESPAWN;
    std::shared_ptr<Net::Message> message = respawnMessage;
    m_networkController->sendMessage(0, message, true);
}
