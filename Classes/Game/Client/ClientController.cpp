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
#include "PlayAudioEvent.h"
#include "PlayerLogic.h"
#include "Pseudo3DItem.h"
#include "Pseudo3DParticle.h"
#include "ReplayModel.h"
#include "SharedConstants.h"
#include "SnapshotModel.h"
#include "Utils/CollisionUtils.h"
#include "WeaponConstants.h"

#include "ShutdownClientCommand.h"
#include "ShutdownLocalServerCommand.h"

#include "InventoryLayer.h"
#include "ShootToContinueLayer.h"
#include "ExitGameLayer.h"

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
    m_networkController->removeMessageCallback(MessageTypes::MESSAGE_TYPE_SERVER_STARTGAME);
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
    m_networkController->receiveMessages();

    m_networkController->update(deltaTime);

    if (m_clientModel->getMode() == ClientMode::CLIENT_MODE_NETWORK)
    {

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

            m_networkController->sendMessage(0, networkData);

            processInput = true;
        }
    }

    m_networkController->sendMessages();

    if (m_clientModel->getGameStarted())
    {
        m_gameModel->setDeltaAccumulator(deltaAccumulator);
        updateGame(deltaTime, processInput);
        
        if (m_hudView->getViewLayer())
        {
            if (m_hudView->getViewLayer()->getDescriptor() == InventoryLayer::DESCRIPTOR)
            {
                auto inventoryLayer = std::dynamic_pointer_cast<InventoryLayer>(m_hudView->getViewLayer());
                inventoryLayer->setData(m_clientModel->getLocalPlayerID());
            }
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
            predictLocalMovement(toSnapshot, fromSnapshot);
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
        
        const cocos2d::Vec2 direction = cocos2d::Vec2(input->directionX, input->directionY);
        const cocos2d::Vec2 velocity = PlayerLogic::getMovementVelocityForInput(direction, input->run);
        const float angularVelocity = 0.f;
        MovementIntegrator::integratePosition(m_gameModel->getFrameTime(),
                                              localPlayerEntityID,
                                              toEntitySnapshot,
                                              velocity,
                                              angularVelocity,
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
        if (m_hudView->getViewLayer()->getDescriptor() == InventoryLayer::DESCRIPTOR)
        {
            m_hudView->removeViewLayer();
        }
        return;
    }
    auto inventoryLayer = Injector::globalInjector().instantiateUnmapped<InventoryLayer,
        SnapshotModel>();
    inventoryLayer->initialize();
    inventoryLayer->setItemPickupCallback([this](const EntityType type,
                                                 const uint16_t amount,
                                                 const uint16_t entityID){
        m_inputModel->setInteract(true);
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
    
    bool blockInput = !m_clientModel->getLocalPlayerAlive();
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
        auto shootToContinueLayer = Injector::globalInjector().instantiateUnmapped<ShootToContinueLayer,
        InputModel>();
        shootToContinueLayer->setup("YOU DIED",
                                    "Press Fire to respawn",
                                    [this](){
            // Send respawn message to server
            std::shared_ptr<ClientStateUpdateMessage> respawnMessage = std::make_shared<ClientStateUpdateMessage>();
            respawnMessage->state = ClientState::PLAYER_RESPAWN;
            std::shared_ptr<Net::Message> message = respawnMessage;
            m_networkController->sendMessage(0, message, true);
        });
        m_hudView->setViewLayer(shootToContinueLayer);
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
