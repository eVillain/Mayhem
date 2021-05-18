#ifndef InitClientCommand_h
#define InitClientCommand_h

#include "Core/Command.h"
#include "Game/Shared/GameMode.h"

class InitClientCommand : public Command
{
public:
    enum Mode {
        HOST,
        CLIENT,
        FAKE_CLIENT,
    };
    
    InitClientCommand(const Mode mode,
                      const GameMode::Config& config);

    virtual bool run() override;

private:
    Mode m_mode;
    GameMode::Config m_config;
    
    void mapDependencies();
};

#endif /* InitClientCommand_h */
