#include "InitLocalServerCommand.h"

#include "cocos2d.h"
#include "EntitiesController.h"
#include "EntitiesModel.h"
#include "GameController.h"
#include "FrameCache.h"
#include "InputCache.h"
#include "Core/Injector.h"
#include "ServerController.h"
#include "FakeNetworkController.h"
#include "FakeNet.h"
#include "Core/Dispatcher.h"
#include "SpawnPlayerEvent.h"
#include "LevelModel.h"
#include "LoadStaticEntityDataCommand.h"
#include "EntityDataModel.h"
#include "GameModel.h"

InitLocalServerCommand::InitLocalServerCommand(const GameMode::Config& config)
: m_config(config)
{
}

bool InitLocalServerCommand::run()
{
    initControllers();
    
    return true;
}

void InitLocalServerCommand::initControllers()
{
    if (!EntityDataModel::hasStaticEntityData())
    {
        LoadStaticEntityDataCommand loadStaticEntityData;
        loadStaticEntityData.run();
        EntityDataModel::setStaticEntityData(loadStaticEntityData.itemDataMap);
    }
    
    entitiesModel = std::make_shared<EntitiesModel>();
    entitiesController = std::make_shared<EntitiesController>(entitiesModel);
    entitiesController->initialize();
    auto levelModel = std::make_shared<LevelModel>();
    levelModel->loadLevel(m_config.level);
    auto gameModel = std::make_shared<GameModel>();
    gameModel->setTickRate(m_config.tickRate);
    gameController = std::make_shared<GameController>(entitiesController, entitiesModel, levelModel);
    gameController->setGameMode(m_config, true);
    frameCache = std::make_shared<FrameCache>();
    inputCache = std::make_shared<InputCache>();

    auto fakeNet = Injector::globalInjector().getInstance<FakeNet>();
    networkController = std::make_shared<FakeNetworkController>(fakeNet);
    networkController->initialize(NetworkMode::HOST);
    serverController = std::make_shared<ServerController>(gameController, levelModel, gameModel, networkController, frameCache, inputCache);
    serverController->initDebugStuff();
    
    Injector::globalInjector().mapInstance<ServerController>(serverController);
}
