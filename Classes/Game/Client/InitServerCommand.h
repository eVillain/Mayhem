#ifndef InitServerCommand_h
#define InitServerCommand_h

#include "Core/Command.h"
#include "GameMode.h"

class InitServerCommand : public Command
{
public:
    InitServerCommand(const GameMode::Config& config);

    virtual bool run() override;

private:
    GameMode::Config m_config;
    
    void initControllers();
};

#endif /* InitServerCommand_h */
