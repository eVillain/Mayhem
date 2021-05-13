#ifndef InitLocalServerCommand_h
#define InitLocalServerCommand_h

#include "Core/Command.h"
#include "Game/Server/GameMode.h"

class EntitiesModel;
class EntitiesController;
class FrameCache;
class InputCache;
class GameController;
class ServerController;
class FakeNetworkController;

class InitLocalServerCommand : public Command
{
public:
    InitLocalServerCommand(const GameMode::Config& config);

    virtual bool run() override;

    std::shared_ptr<EntitiesModel> entitiesModel;
    std::shared_ptr<EntitiesController> entitiesController;
    std::shared_ptr<FrameCache> frameCache;
    std::shared_ptr<InputCache> inputCache;
    std::shared_ptr<GameController> gameController;
    std::shared_ptr<ServerController> serverController;
    std::shared_ptr<FakeNetworkController> networkController;
private:
    GameMode::Config m_config;
    void initControllers();
};

#endif /* InitLocalServerCommand_h */
