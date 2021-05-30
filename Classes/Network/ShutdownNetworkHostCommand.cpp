#include "ShutdownNetworkHostCommand.h"

#include "Core/Injector.h"
#include "NetworkConstants.h"
#include "NetworkModel.h"
#include "GameSettings.h"
#include "NetworkController.h"
#include "NetworkHostViewController.h"
#include "ClientModel.h"

ShutdownNetworkHostCommand::ShutdownNetworkHostCommand()
{
}

bool ShutdownNetworkHostCommand::run()
{
    Injector& injector = Injector::globalInjector();

    injector.removeMapping<NetworkModel>();
    injector.removeMapping<NetworkController>();
    injector.removeMapping<INetworkController>();
    injector.removeMapping<ClientModel>();
    injector.removeMapping<NetworkHostViewController>();

    return true;
}
