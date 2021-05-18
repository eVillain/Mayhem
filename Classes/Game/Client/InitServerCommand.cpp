#include "InitServerCommand.h"

#include "cocos2d.h"
#include "Core/Injector.h"
#include "EntitiesController.h"
#include "EntitiesModel.h"
#include "EntityDataModel.h"
#include "FrameCache.h"
#include "Game/Client/InputModel.h"
#include "GameController.h"
#include "GameModel.h"
#include "GameScene.h"
#include "GameSettings.h"
#include "GameView.h"
#include "InputCache.h"
#include "InputController.h"
#include "LevelModel.h"
#include "LoadStaticEntityDataCommand.h"
#include "NetworkController.h"
#include "NetworkModel.h"
#include "ServerController.h"

InitServerCommand::InitServerCommand(const GameMode::Config& config)
: m_config(config)
{
}

bool InitServerCommand::run()
{
    if (!EntityDataModel::hasStaticEntityData())
    {
        LoadStaticEntityDataCommand loadStaticEntityData;
        loadStaticEntityData.run();
        EntityDataModel::setStaticEntityData(loadStaticEntityData.itemDataMap);
    }
    
    mapDependencies();
        
    Injector& injector = Injector::globalInjector();

    auto entitiesModel = injector.getInstance<EntitiesModel>();
    auto entitiesController = injector.getInstance<EntitiesController>();
    entitiesController->initialize();
    auto levelModel = injector.getInstance<LevelModel>();
    levelModel->loadLevel(m_config.level);
    auto gameController = injector.getInstance<GameController>();
    gameController->setGameMode(m_config, true);

    if (!injector.hasMapping<INetworkController>())
    {
        injector.mapInterfaceToType<INetworkController, NetworkController>();
        injector.mapSingleton<NetworkController,
            NetworkModel, GameSettings>();
    }
    if (!injector.hasMapping<ServerController>())
    {
        auto networkController = injector.getInstance<NetworkController>();
//        networkController->initialize(NetworkMode::HOST);
        auto frameCache = injector.getInstance<FrameCache>();
        auto inputCache = injector.getInstance<InputCache>();
        auto gameModel = injector.instantiateUnmapped<GameModel>();
        gameModel->setConfig(m_config);

        auto serverController = std::make_shared<ServerController>(gameController, levelModel, gameModel, networkController, frameCache, inputCache);
        Injector::globalInjector().mapInstance<ServerController>(serverController);
    }
    
    return true;
}

void InitServerCommand::mapDependencies()
{
    Injector& injector = Injector::globalInjector();
    
    if (!injector.hasMapping<EntitiesModel>())
    {
        injector.mapSingleton<EntitiesModel>();
    }
    if (!injector.hasMapping<EntitiesController>())
    {
        injector.mapSingleton<EntitiesController,
            EntitiesModel>();
    }
    if (!injector.hasMapping<LevelModel>())
    {
        injector.mapSingleton<LevelModel>();
    }
    if (!injector.hasMapping<GameController>())
    {
        injector.mapSingleton<GameController,
            EntitiesController, EntitiesModel, LevelModel>();
    }
    if (!injector.hasMapping<FrameCache>())
    {
        injector.mapSingleton<FrameCache>();
    }
    if (!injector.hasMapping<InputCache>())
    {
        injector.mapSingleton<InputCache>();
    }
}
