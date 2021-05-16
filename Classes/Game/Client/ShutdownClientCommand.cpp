#include "ShutdownClientCommand.h"

//#include "InitLocalServerCommand.h"
//#include "InitServerCommand.h"
//#include "LoadLevelCommand.h"
//#include "LoadStaticEntityDataCommand.h"

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
#include "ServerController.h"

ShutdownClientCommand::ShutdownClientCommand()
{
}

bool ShutdownClientCommand::run()
{
    Injector& injector = Injector::globalInjector();
    auto levelModel = injector.getInstance<LevelModel>();
    levelModel->unloadLevel();
    
    auto gameModel = injector.getInstance<GameModel>();
    gameModel->reset();
    
    auto hudView = injector.getInstance<HUDView>();
    hudView->shutdown();

    auto gameView = injector.getInstance<GameView>();
    gameView->shutdown();
    
    auto particlesController = injector.getInstance<ParticlesController>();
    particlesController->shutdown();

//    auto audioController = injector.getInstance<AudioController>();
//    audioController->shutdown(); // Doesnt need a shutdown method yet

    auto lightController = injector.getInstance<LightController>();
    lightController->shutdown();
    
    auto clientController = injector.getInstance<ClientController>();
    
    auto inputController = injector.getInstance<InputController>();
    inputController->shutdown();
    
    if (injector.hasMapping<ServerController>())
    {
        auto serverController = injector.getInstance<ServerController>();
        serverController->stop();
    }
    
    // Show cursor
    auto director = cocos2d::Director::getInstance();
    auto glview = director->getOpenGLView();
    glview->setCursorVisible(true);

    return true;
}
