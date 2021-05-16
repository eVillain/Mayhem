#ifndef ShutdownClientCommand_h
#define ShutdownClientCommand_h

#include "Core/Command.h"

class ShutdownClientCommand : public Command
{
public:
    ShutdownClientCommand();

    virtual bool run() override;
};

#endif /* ShutdownClientCommand_h */
