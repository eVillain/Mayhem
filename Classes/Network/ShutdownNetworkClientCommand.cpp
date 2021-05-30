#include "ShutdownNetworkClientCommand.h"

#include "Core/Injector.h"
#include "GameSettings.h"
#include "NetworkModel.h"
#include "NetworkController.h"
#include "NetworkConstants.h"
#include "NetworkClientViewController.h"
#include "ClientModel.h"

ShutdownNetworkClientCommand::ShutdownNetworkClientCommand()
{
}

bool ShutdownNetworkClientCommand::run()
{
    Injector& injector = Injector::globalInjector();

    injector.removeMapping<NetworkModel>();
    injector.removeMapping<NetworkController>();
    injector.removeMapping<INetworkController>();
    injector.removeMapping<ClientModel>();
    injector.removeMapping<NetworkClientViewController>();

    return true;
}
