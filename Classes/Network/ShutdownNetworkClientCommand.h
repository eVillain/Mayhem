#ifndef ShutdownNetworkClientCommand_h
#define ShutdownNetworkClientCommand_h

#include "Core/Command.h"

class NetworkClientView;

class ShutdownNetworkClientCommand : public Command
{
public:
    ShutdownNetworkClientCommand();

    virtual bool run() override;
};

#endif /* ShutdownNetworkClientCommand_h */
