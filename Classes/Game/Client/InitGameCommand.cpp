#include "InitGameCommand.h"

#include "cocos2d.h"
#include "EntitiesController.h"
#include "EntitiesModel.h"
#include "EntityDataModel.h"
#include "GameScene.h"
#include "GameView.h"
#include "GameController.h"
#include "Core/Injector.h"
#include "LevelModel.h"
#include "LoadStaticEntityDataCommand.h"

InitGameCommand::InitGameCommand()
{
}

bool InitGameCommand::run()
{
    initControllers();

    Injector& injector = Injector::globalInjector();
    
    auto entitiesController = injector.getInstance<EntitiesController>();
    entitiesController->initialize();

    return true;
}

void InitGameCommand::initControllers()
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
    
    if (!EntityDataModel::hasStaticEntityData())
    {
        LoadStaticEntityDataCommand loadStaticEntityData;
        loadStaticEntityData.run();
        EntityDataModel::setStaticEntityData(loadStaticEntityData.itemDataMap);
    }

    if (!injector.hasMapping<GameController>())
    {
        injector.mapSingleton<GameController,
            EntitiesController, EntitiesModel, LevelModel>();
    }
}
