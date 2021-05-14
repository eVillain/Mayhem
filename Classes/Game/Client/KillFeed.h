#ifndef KillFeed_h
#define KillFeed_h

#include "EntityConstants.h"
#include "cocos2d.h"
#include "cocos-ext.h"
#include <queue>

class KillFeed : public cocos2d::Node
{
public:
    KillFeed();
    CREATE_FUNC(KillFeed);
    
    void onPlayerKilled(const std::string& deadPlayerName,
                        const std::string& killerPlayerName,
                        const EntityType killerType,
                        const bool headshot);
    
    void setMaxLabels(const size_t max) { m_maxLabels = max; }

private:
    struct KillFeedData {
        const std::string deadPlayerName;
        const std::string killerPlayerName;
        const EntityType killerType;
        const bool headshot;
    };
    
    size_t m_maxLabels;
    std::queue<KillFeedData> m_queue;
    
    void popNextLabelFromQueue();
    float findNextLabelPosition() const;
};

#endif /* KillFeed_h */
