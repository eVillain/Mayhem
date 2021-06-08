#ifndef SHUTDOWN_MAIN_MENU_COMMAND_H
#define SHUTDOWN_MAIN_MENU_COMMAND_H

#include "Core/Command.h"

class ShutdownMainMenuCommand : public Command
{
public:
    virtual bool run() override;
};

#endif /* SHUTDOWN_MAIN_MENU_COMMAND_H */
