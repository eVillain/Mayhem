#include "ShutdownMainMenuCommand.h"

#include "Core/Injector.h"
#include "MainMenuController.h"
#include "GameSettings.h"
#include "InputController.h"
#include "InputModel.h"
#include "cocos2d.h"

bool ShutdownMainMenuCommand::run()
{
    Injector& injector = Injector::globalInjector();

    auto inputController = injector.getInstance<InputController>();
    inputController->shutdown();
    
    injector.removeMapping<InputController>();
    injector.removeMapping<InputModel>();

    return true;
}
