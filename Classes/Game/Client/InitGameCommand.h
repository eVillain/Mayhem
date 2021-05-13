#ifndef InitGameCommand_h
#define InitGameCommand_h

#include "Core/Command.h"

class InitGameCommand : public Command
{
public:
    InitGameCommand();

    virtual bool run() override;

private:
    void initControllers();
};

#endif /* InitGameCommand_h */
