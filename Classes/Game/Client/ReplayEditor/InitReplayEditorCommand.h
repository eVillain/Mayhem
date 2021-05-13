#ifndef INIT_REPLAY_EDITOR_COMMAND_H
#define INIT_REPLAY_EDITOR_COMMAND_H

#include "Core/Command.h"

class InitReplayEditorCommand : public Command
{
public:
    virtual bool run() override;
};

#endif /* INIT_REPLAY_EDITOR_COMMAND_H */
