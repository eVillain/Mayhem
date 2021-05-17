#ifndef InitLocalServerCommand_h
#define InitLocalServerCommand_h

#include "Core/Command.h"
#include "Game/Server/GameMode.h"

class InitLocalServerCommand : public Command
{
public:
    InitLocalServerCommand(const GameMode::Config& config);

    virtual bool run() override;

private:
    GameMode::Config m_config;
    void mapDependencies();
};

#endif /* InitLocalServerCommand_h */
