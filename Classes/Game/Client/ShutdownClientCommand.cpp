#include "ShutdownClientCommand.h"

//#include "InitLocalServerCommand.h"
//#include "InitServerCommand.h"
//#include "LoadLevelCommand.h"
//#include "LoadStaticEntityDataCommand.h"

#include "InitMainMenuCommand.h"

#include "AudioModel.h"
#include "AudioController.h"
#include "ClientController.h"
#include "ClientModel.h"
#include "cocos2d.h"
#include "Core/Injector.h"
#include "EntityDataModel.h"
#include "FakeNet.h"
#include "FakeNetworkController.h"
#include "Game/Client/InputModel.h"
#include "GameModel.h"
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

ShutdownClientCommand::ShutdownClientCommand()
{
}

bool ShutdownClientCommand::run()
{
    Injector& injector = Injector::globalInjector();
    
    auto inputController = injector.getInstance<InputController>();
    inputController->shutdown();
    auto lightController = injector.getInstance<LightController>();
    lightController->shutdown();
    auto particlesController = injector.getInstance<ParticlesController>();
    particlesController->shutdown();

    injector.removeMapping<GameModel>();
    injector.removeMapping<LevelModel>();
    injector.removeMapping<HUDView>();
    injector.removeMapping<GameViewController>();
    injector.removeMapping<GameView>();
    injector.removeMapping<SnapshotModel>();
    injector.removeMapping<ReplayModel>();
    injector.removeMapping<ClientModel>();
    injector.removeMapping<ParticlesController>();
    injector.removeMapping<AudioController>();
    injector.removeMapping<AudioModel>();
    injector.removeMapping<LightModel>();
    injector.removeMapping<LightController>();
    injector.removeMapping<ClientController>();
//    injector.removeMapping<InputModel>();
//    injector.removeMapping<InputController>();
    injector.removeMapping<INetworkController>();
    injector.removeMapping<FakeNet>();

    InitMainMenuCommand initMainMenu;
    initMainMenu.run();
    
    return true;
}
