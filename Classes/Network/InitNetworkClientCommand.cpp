#include "InitNetworkClientCommand.h"

#include "Core/Injector.h"
#include "NetworkModel.h"
#include "NetworkController.h"
#include "NetworkConstants.h"
#include "NetworkClientViewController.h"
#include "cocos2d.h"

InitNetworkClientCommand::InitNetworkClientCommand(NetworkClientView* view)
: m_view(view)
{
}

bool InitNetworkClientCommand::run()
{
    Injector& injector = Injector::globalInjector();

    if (!injector.hasMapping<NetworkModel>())
    {
        injector.mapSingleton<NetworkModel>();
    }
    if (!injector.hasMapping<NetworkClientViewController>())
    {
        injector.mapSingleton<NetworkClientViewController>();
    }
    if (!injector.hasMapping<INetworkController>())
    {
        injector.mapSingleton<NetworkController>();
        injector.mapInterfaceToType<INetworkController, NetworkController>();
    }

    auto networkController = injector.getInstance<INetworkController>();
    networkController->initialize(NetworkMode::CLIENT);
    
    auto networkClientController = injector.getInstance<NetworkClientViewController>();
    networkClientController->initialize();
    networkClientController->setView(m_view);

    return true;
}
