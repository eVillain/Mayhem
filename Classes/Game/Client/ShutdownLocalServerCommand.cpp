#include "ShutdownLocalServerCommand.h"

#include "cocos2d.h"
#include "Core/Injector.h"
#include "EntitiesController.h"
#include "EntitiesModel.h"
#include "FakeNet.h"
#include "FakeNetworkController.h"
#include "FrameCache.h"
#include "GameController.h"
#include "GameModel.h"
#include "InputCache.h"
#include "LevelModel.h"
#include "ServerController.h"

ShutdownLocalServerCommand::ShutdownLocalServerCommand()
{
}

bool ShutdownLocalServerCommand::run()
{
    Injector& injector = Injector::globalInjector();
    
    injector.removeMapping<LevelModel>();
    injector.removeMapping<GameModel>();
    injector.removeMapping<FakeNet>();
    injector.removeMapping<FakeNetworkController>();

    injector.removeMapping<EntitiesModel>();
    injector.removeMapping<EntitiesController>();
    injector.removeMapping<GameController>();
    injector.removeMapping<FrameCache>();
    injector.removeMapping<InputCache>();
    injector.removeMapping<ServerController>();

    return true;
}

