#include "InitMainMenuCommand.h"

#include "Core/Injector.h"
#include "MainMenuController.h"
#include "GameSettings.h"
#include "InputController.h"
#include "InputModel.h"
#include "cocos2d.h"

bool InitMainMenuCommand::run()
{
    Injector& injector = Injector::globalInjector();

    if (!injector.hasMapping<GameSettings>())
    {
        injector.mapSingleton<GameSettings>();
        
        auto gameSettings = injector.getInstance<GameSettings>();
        gameSettings->load(GameSettings::DEFAULT_SETTINGS_FILE);
    }
    
    if (!injector.hasMapping<InputModel>())
    {
        injector.mapSingleton<InputModel>();
    }
    if (!injector.hasMapping<InputController>())
    {
        injector.mapSingleton<InputController,
            InputModel>();
    }

    // Show cursor
    auto director = cocos2d::Director::getInstance();
    auto glview = director->getOpenGLView();
    glview->setCursorVisible(true);
    
    auto mainMenu = MainMenuController::create();
    
    auto inputController = injector.getInstance<InputController>();
    inputController->initialize(mainMenu);

    cocos2d::Director::getInstance()->replaceScene(mainMenu);
    
    return true;
}
