#include "InitServerCommand.h"

#include "cocos2d.h"
#include "EntitiesController.h"
#include "EntitiesModel.h"
#include "GameScene.h"
#include "GameView.h"
#include "GameController.h"
#include "Core/Injector.h"
#include "InputController.h"
#include "Game/Client/InputModel.h"
#include "FrameCache.h"
#include "InputCache.h"
#include "ServerController.h"
#include "NetworkController.h"
#include "LevelModel.h"
#include "LoadStaticEntityDataCommand.h"
#include "EntityDataModel.h"
#include "GameModel.h"

InitServerCommand::InitServerCommand(const GameMode::Config& config)
: m_config(config)
{
}

bool InitServerCommand::run()
{
    initControllers();

//    Injector& injector = Injector::globalInjector();
//    auto entitiesController = injector.getInstance<EntitiesController>();
//    entitiesController->initialize();

    return true;
}

void InitServerCommand::initControllers()
{
    Injector& injector = Injector::globalInjector();
    
// If we were in "headless" mode we could use the injector
//    if (!injector.hasMapping<EntitiesModel>())
//    {
//        injector.mapSingleton<EntitiesModel>();
//    }
//    if (!injector.hasMapping<EntitiesController>())
//    {
//        injector.mapSingleton<EntitiesController,
//            EntitiesModel>();
//    }
//
//    if (!injector.hasMapping<GameController>())
//    {
//        injector.mapSingleton<GameController,
//        EntitiesController, EntitiesModel>();
//    }
//
//    if (!injector.hasMapping<FrameCache>())
//    {
//        injector.mapSingleton<FrameCache>();
//    }
//    if (!injector.hasMapping<InputCache>())
//    {
//        injector.mapSingleton<InputCache>();
//    }
    
    if (!EntityDataModel::hasStaticEntityData())
    {
        LoadStaticEntityDataCommand loadStaticEntityData;
        loadStaticEntityData.run();
        EntityDataModel::setStaticEntityData(loadStaticEntityData.itemDataMap);
    }
    if (!injector.hasMapping<INetworkController>())
    {
        injector.mapInterfaceToType<INetworkController, NetworkController>();
        injector.mapSingleton<NetworkController>();
    }
    if (!injector.hasMapping<ServerController>())
    {
        auto entitiesModel = std::make_shared<EntitiesModel>();
        auto entitiesController = std::make_shared<EntitiesController>(entitiesModel);
        entitiesController->initialize();
        auto levelModel = std::make_shared<LevelModel>();
        levelModel->loadLevel(m_config.level);
        auto gameModel = std::make_shared<GameModel>();
        gameModel->setConfig(m_config);
        auto gameController = std::make_shared<GameController>(entitiesController, entitiesModel, levelModel);
        gameController->setGameMode(m_config, true);
        auto frameCache = std::make_shared<FrameCache>();
        auto inputCache = std::make_shared<InputCache>();
        auto networkController = injector.getInstance<NetworkController>();
        auto serverController = std::make_shared<ServerController>(gameController, levelModel, gameModel, networkController, frameCache, inputCache);
        Injector::globalInjector().mapInstance<ServerController>(serverController);
    }
}
