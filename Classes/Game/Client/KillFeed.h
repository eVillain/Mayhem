#ifndef KillFeed_h
#define KillFeed_h

#include "cocos2d.h"
#include "cocos-ext.h"

#include "EntityConstants.h"

class KillFeed : public cocos2d::Node
{
public:
    KillFeed();
    CREATE_FUNC(KillFeed);
    
    void onPlayerKilled(const std::string& deadPlayerName,
                        const std::string& killerPlayerName,
                        const EntityType killerType,
                        const bool headshot);
};

#endif /* KillFeed_h */
