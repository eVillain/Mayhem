#ifndef ShutdownNetworkHostCommand_h
#define ShutdownNetworkHostCommand_h

#include "Core/Command.h"

class ShutdownNetworkHostCommand : public Command
{
public:
    ShutdownNetworkHostCommand();
    
    virtual bool run() override;
};

#endif /* ShutdownNetworkHostCommand_h */
