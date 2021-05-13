#include "InitMainMenuCommand.h"
#include "Core/Injector.h"
#include "MainMenuController.h"
#include "GameSettings.h"
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

    // Create the main menu scene - it's an autorelease object
    auto mainMenu = MainMenuController::create();
    
    // run
    if (!cocos2d::Director::getInstance()->getRunningScene())
    {
        cocos2d::Director::getInstance()->runWithScene(mainMenu);
    }
    else
    {
        cocos2d::Director::getInstance()->replaceScene(mainMenu);
    }
    
    return true;
}
