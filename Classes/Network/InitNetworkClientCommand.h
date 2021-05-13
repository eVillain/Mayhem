#ifndef InitNetworkClientCommand_h
#define InitNetworkClientCommand_h

#include "Core/Command.h"

class NetworkClientView;

class InitNetworkClientCommand : public Command
{
public:
    InitNetworkClientCommand(NetworkClientView* view);
    virtual bool run() override;
    
private:
    NetworkClientView* m_view;
};

#endif /* InitNetworkClientCommand_h */
