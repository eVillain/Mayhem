#ifndef INIT_MAIN_MENU_COMMAND_H
#define INIT_MAIN_MENU_COMMAND_H

#include "Core/Command.h"

class InitMainMenuCommand : public Command
{
public:
    virtual bool run() override;
};

#endif /* INIT_MAIN_MENU_COMMAND_H */
