#ifndef ShutdownLocalServerCommand_h
#define ShutdownLocalServerCommand_h

#include "Core/Command.h"

class ShutdownLocalServerCommand : public Command
{
public:
    ShutdownLocalServerCommand();

    virtual bool run() override;
};

#endif /* ShutdownLocalServerCommand_h */
