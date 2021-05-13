#ifndef LoadLevelCommand_h
#define LoadLevelCommand_h

#include "Core/Command.h"
#include <string>

class LoadLevelCommand : public Command
{
public:
    LoadLevelCommand(const std::string& levelFile);

    virtual bool run() override;
    
private:
    std::string m_levelFile;
};

#endif /* LoadLevelCommand_h */
