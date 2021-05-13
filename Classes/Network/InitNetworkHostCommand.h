#ifndef InitNetworkHostCommand_h
#define InitNetworkHostCommand_h

#include "Core/Command.h"

class NetworkHostView;

class InitNetworkHostCommand : public Command
{
public:
    InitNetworkHostCommand(NetworkHostView* view);
    
    virtual bool run() override;
    
private:
    NetworkHostView* m_view;
};

#endif /* InitNetworkHostCommand_h */
