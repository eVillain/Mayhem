#include "InitNetworkHostCommand.h"

#include "Core/Injector.h"
#include "NetworkConstants.h"
#include "NetworkModel.h"
#include "NetworkController.h"
#include "NetworkHostViewController.h"
#include "cocos2d.h"

InitNetworkHostCommand::InitNetworkHostCommand(NetworkHostView* view)
: m_view(view)
{
}

bool InitNetworkHostCommand::run()
{
    Injector& injector = Injector::globalInjector();

    if (!injector.hasMapping<NetworkModel>())
    {
        injector.mapSingleton<NetworkModel>();
    }
    if (!injector.hasMapping<INetworkController>())
    {
        injector.mapSingleton<NetworkController>();
        injector.mapInterfaceToType<INetworkController, NetworkController>();
    }
    if (!injector.hasMapping<NetworkHostViewController>())
    {
        injector.mapSingleton<NetworkHostViewController>();
    }
    
    auto networkController = injector.getInstance<INetworkController>();
    networkController->initialize(NetworkMode::HOST);

    auto networkHostController = injector.getInstance<NetworkHostViewController>();
    networkHostController->initialize();
    networkHostController->setView(m_view);

    return true;
}
