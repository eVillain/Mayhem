#include "LoadLevelCommand.h"

#include "Core/Injector.h"
#include "LevelModel.h"

LoadLevelCommand::LoadLevelCommand(const std::string& levelFile)
: m_levelFile(levelFile)
{
}

bool LoadLevelCommand::run()
{
    Injector& injector = Injector::globalInjector();
    
    if (!injector.hasMapping<LevelModel>())
    {
        injector.mapSingleton<LevelModel>();
    }
    
    auto levelModel = injector.getInstance<LevelModel>();
    levelModel->loadLevel(m_levelFile);

    return true;
}
