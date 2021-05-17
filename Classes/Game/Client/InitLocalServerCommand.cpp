#include "InitLocalServerCommand.h"

#include "cocos2d.h"
#include "Core/Dispatcher.h"
#include "Core/Injector.h"
#include "EntitiesController.h"
#include "EntitiesModel.h"
#include "EntityDataModel.h"
#include "FakeNet.h"
#include "FakeNetworkController.h"
#include "FrameCache.h"
#include "GameController.h"
#include "GameModel.h"
#include "InputCache.h"
#include "LevelModel.h"
#include "LoadStaticEntityDataCommand.h"
#include "ServerController.h"

InitLocalServerCommand::InitLocalServerCommand(const GameMode::Config& config)
: m_config(config)
{
}

bool InitLocalServerCommand::run()
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

    if (!injector.hasMapping<ServerController>())
    {
        auto networkController = injector.instantiateUnmapped<FakeNetworkController,
            FakeNet>();
        networkController->initialize(NetworkMode::HOST);
        auto frameCache = injector.getInstance<FrameCache>();
        auto inputCache = injector.getInstance<InputCache>();
        auto gameModel = injector.instantiateUnmapped<GameModel>();
        gameModel->setTickRate(m_config.tickRate);

        auto serverController = std::make_shared<ServerController>(gameController,
                                                                   levelModel,
                                                                   gameModel,
                                                                   networkController,
                                                                   frameCache,
                                                                   inputCache);
        Injector::globalInjector().mapInstance<ServerController>(serverController);
    }
    
    auto serverController = injector.getInstance<ServerController>();
    serverController->initDebugStuff();
    
    return true;
}

void InitLocalServerCommand::mapDependencies()
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
    if (!injector.hasMapping<FakeNet>())
    {
        injector.mapSingleton<FakeNet>();
    }
}
