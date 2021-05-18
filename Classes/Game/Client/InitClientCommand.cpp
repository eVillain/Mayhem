#include "InitClientCommand.h"

#include "InitLocalServerCommand.h"
#include "InitServerCommand.h"
#include "LoadLevelCommand.h"
#include "LoadStaticEntityDataCommand.h"

#include "AudioController.h"
#include "ClientController.h"
#include "ClientModel.h"
#include "cocos2d.h"
#include "Core/Injector.h"
#include "EntityDataModel.h"
#include "FakeNet.h"
#include "FakeNetworkController.h"
#include "Game/Client/InputModel.h"
#include "GameScene.h"
#include "GameSettings.h"
#include "GameView.h"
#include "GameViewController.h"
#include "HUDView.h"
#include "InputController.h"
#include "LevelModel.h"
#include "LightController.h"
#include "LightModel.h"
#include "NetworkController.h"
#include "ParticlesController.h"
#include "ReplayModel.h"
#include "SnapshotModel.h"
#include "GameModel.h"

InitClientCommand::InitClientCommand(const Mode mode,
                                     const GameMode::Config& config)
: m_mode(mode)
, m_config(config)
{
}

bool InitClientCommand::run()
{
    if (!EntityDataModel::hasStaticEntityData())
    {
        LoadStaticEntityDataCommand loadStaticEntityData;
        loadStaticEntityData.run();
        EntityDataModel::setStaticEntityData(loadStaticEntityData.itemDataMap);
    }
    
    mapDependencies();

    LoadLevelCommand loadLevel(m_config.level);
    loadLevel.run();
    
    Injector& injector = Injector::globalInjector();
    auto levelModel = injector.getInstance<LevelModel>();

    auto gameModel = injector.getInstance<GameModel>();
    gameModel->setConfig(m_config);
    
    auto hudView = injector.getInstance<HUDView>();
    hudView->initialize();

    auto gameView = injector.getInstance<GameView>();
    gameView->initialize();
    gameView->setTileMap(levelModel->getTileMap());
    
    auto particlesController = injector.getInstance<ParticlesController>();
    particlesController->initialize();

    auto audioController = injector.getInstance<AudioController>();
    audioController->initialize();

    auto lightController = injector.getInstance<LightController>();
    lightController->initialize();
    
    auto clientController = injector.getInstance<ClientController>();
    auto networkController = injector.getInstance<INetworkController>();
    networkController->initialize(NetworkMode::CLIENT);
    networkController->setDeltaDataCallback(std::bind(&ClientController::getDeltaData, clientController, std::placeholders::_1));

    if (m_mode == HOST)
    {
        clientController->setMode(ClientMode::CLIENT_MODE_LOCAL);
    }
    else if (m_mode == CLIENT || m_mode == FAKE_CLIENT)
    {
        clientController->setMode(ClientMode::CLIENT_MODE_NETWORK);
    }

    // Create the game scene - it's an autorelease object
    auto gameScene = GameScene::create();
    
    auto inputController = injector.getInstance<InputController>();
    inputController->initialize(gameScene);
    
    // Add game views to scene
    auto gameSettings = injector.getInstance<GameSettings>();
    const cocos2d::Value& deferredRenderSetting = gameSettings->getValue(GameView::SETTING_RENDER_DEFERRED, cocos2d::Value(true));
    if (deferredRenderSetting.asBool())
    {
        gameScene->addChild(gameView->getRenderTexture(), -1);
    }

    gameScene->addChild(gameView->getView());

    // Debug views for light maps
    const cocos2d::Value& renderLightingSetting = gameSettings->getValue(LightController::SETTING_RENDER_LIGHTING, cocos2d::Value(true));
    if (renderLightingSetting.asBool())
    {
        // Debug various lighting stages by drawing texture to screen
//        gameScene->addChild(lightController->getOccluderTexture(), 2);
//        gameScene->addChild(lightController->getOccluderSliceTexture(), 2);
//        gameScene->addChild(lightController->getDistortedTexture(), 2);
//        gameScene->addChild(lightController->getMinimumDistanceTexture(), 3);
//        gameScene->addChild(lightController->getShadowTexture(), 2);
        // The actual lightmap, this needs to be added here for lighting to work
        const cocos2d::Value& postProcessSetting = gameSettings->getValue(GameView::SETTING_RENDER_POSTPROCESS, cocos2d::Value(true));
        if (!postProcessSetting.asBool())
        {
            // No post-processing, apply lighting directly on top of game scene
            gameScene->addChild(lightController->getLightMapTexture(), 2);
        }
        gameView->getSelfLightingNode()->removeFromParent();
        gameScene->addChild(gameView->getSelfLightingNode(), 3);
    }
    
    gameScene->addChild(hudView->getRoot(), 3);
    gameScene->addChild(gameView->getDebugDrawNode(), 10);
    gameScene->addChild(gameView->getDebugLabelNode(), 11);

    // Hide cursor
    auto director = cocos2d::Director::getInstance();
    auto glview = director->getOpenGLView();
    glview->setCursorVisible(false);
    
    // run
    cocos2d::Director::getInstance()->replaceScene(gameScene);
    
    return true;
}

void InitClientCommand::mapDependencies()
{
    Injector& injector = Injector::globalInjector();

    if (!injector.hasMapping<GameView>())
    {
        injector.mapSingleton<GameView, GameSettings>();
    }
    
    if (!injector.hasMapping<ParticlesController>())
    {
        injector.mapSingleton<ParticlesController>();
    }
    if (!injector.hasMapping<AudioController>())
    {
        injector.mapSingleton<AudioController,
            GameSettings>();
    }

    if (!injector.hasMapping<LightController>())
    {
        if (!injector.hasMapping<LightModel>())
        {
            injector.mapSingleton<LightModel>();
        }
        injector.mapSingleton<LightController,
            LightModel, GameSettings>();
    }

    if (!injector.hasMapping<InputController>())
    {
        if (!injector.hasMapping<InputModel>())
        {
            injector.mapSingleton<InputModel>();
        }
        injector.mapSingleton<InputController,
            InputModel>();
    }
    
    if (!injector.hasMapping<SnapshotModel>())
    {
        injector.mapSingleton<SnapshotModel>();
    }
    if (!injector.hasMapping<ReplayModel>())
    {
        injector.mapSingleton<ReplayModel>();
    }
    if (!injector.hasMapping<GameModel>())
    {
        injector.mapSingleton<GameModel>();
    }
    if (!injector.hasMapping<LevelModel>())
    {
        injector.mapSingleton<LevelModel>();
    }
    if (!injector.hasMapping<ClientModel>())
    {
        injector.mapSingleton<ClientModel>();
    }
    
    if (!injector.hasMapping<HUDView>())
    {
        injector.mapSingleton<HUDView>();
    }
    
    if (!injector.hasMapping<GameViewController>())
    {
        injector.mapSingleton<GameViewController,
            GameSettings,
            LevelModel,
            GameView,
            InputModel,
            LightController,
            ParticlesController,
            HUDView>();
    }
    
    if (m_mode == FAKE_CLIENT)
    {
        if (!injector.hasMapping<FakeNet>())
        {
            injector.mapSingleton<FakeNet>();
        }
        if (!injector.hasMapping<INetworkController>())
        {
            injector.mapSingleton<FakeNetworkController, FakeNet>();
            injector.mapInterfaceToType<INetworkController, FakeNetworkController>();
        }
        
        InitLocalServerCommand initLocalServer(m_config);
        initLocalServer.run();
    }
    else if (m_mode == HOST)
    {
        InitServerCommand initServer(m_config);
        initServer.run();
    }

    if (!injector.hasMapping<ClientController>())
    {
        injector.mapSingleton<ClientController,
            ClientModel,
            GameSettings,
            GameViewController,
            LevelModel,
            GameModel,
            GameView,
            InputModel,
            SnapshotModel,
            ReplayModel,
            INetworkController,
            LightController,
            HUDView>();
    }
}
