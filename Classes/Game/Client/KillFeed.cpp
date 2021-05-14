#include "KillFeed.h"

#include "HUDHelper.h"

KillFeed::KillFeed()
: m_maxLabels(2)
{
}

void KillFeed::onPlayerKilled(const std::string& deadPlayerName,
                              const std::string& killerPlayerName,
                              const EntityType killerType,
                              const bool headshot)
{
    KillFeedData data = {
        deadPlayerName,
        killerPlayerName,
        killerType,
        headshot
    };
    m_queue.push(data);

    const ssize_t numLabels = getChildrenCount();
    if (numLabels >= m_maxLabels)
    {
        return;
    }
    
    popNextLabelFromQueue();
}

void KillFeed::popNextLabelFromQueue()
{
    if (m_queue.empty())
    {
        return;
    }
    const KillFeedData& data = m_queue.front();
    m_queue.pop();
    
    const std::string feed = data.killerPlayerName + " killed " + data.deadPlayerName;
    auto label = HUDHelper::createLabel3x6(feed, 24);
    label->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
    label->enableOutline(cocos2d::Color4B::BLACK, 1.f);
    
    const float posY = findNextLabelPosition();
    label->setPositionY(-posY);
    addChild(label);
    
    cocos2d::DelayTime* delay = cocos2d::DelayTime::create(4.f);
    cocos2d::FadeOut* fadeOut = cocos2d::FadeOut::create(1.f);
    cocos2d::CallFunc* removeCB = cocos2d::CallFunc::create([this, label](){
        label->removeFromParent();
        popNextLabelFromQueue();
    });
    cocos2d::Sequence* sequence = cocos2d::Sequence::create(delay, fadeOut, removeCB, NULL);
    label->runAction(sequence);
}

float KillFeed::findNextLabelPosition() const
{
    std::map<ssize_t, bool> occupiedSlots;
    for (ssize_t i = 0; i < m_maxLabels; i++)
    {
        occupiedSlots[i] = false;
    }
    
    for (const auto& child : getChildren())
    {
        const float posY = child->getPositionY();
        ssize_t slot = posY / 24.f;
        occupiedSlots[slot] = true;
    }

    for (ssize_t i = 0; i < m_maxLabels; i++)
    {
        if (!occupiedSlots[i])
        {
            return i * 24.f;
        }
    }
    
    return getChildrenCount() * 24.f;
}
