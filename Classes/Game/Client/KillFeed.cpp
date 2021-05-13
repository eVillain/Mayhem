#include "KillFeed.h"

#include "HUDHelper.h"

KillFeed::KillFeed()
{
}

void KillFeed::onPlayerKilled(const std::string& deadPlayerName,
                              const std::string& killerPlayerName,
                              const EntityType killerType,
                              const bool headshot)
{
    const std::string feed = killerPlayerName + " killed " + deadPlayerName;
    auto label = HUDHelper::createLabel3x6(feed, 24);
    label->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
    const ssize_t numLabels = getChildrenCount();
    const float posY = numLabels * 24.f;
    label->setPositionY(-posY);
    addChild(label);
    
    cocos2d::DelayTime* delay = cocos2d::DelayTime::create(1.f);
    cocos2d::FadeOut* fadeOut = cocos2d::FadeOut::create(1.f);
    cocos2d::CallFunc* removeCB = cocos2d::CallFunc::create([label](){
        label->removeFromParent();
    });
    cocos2d::Sequence* sequence = cocos2d::Sequence::create(delay, fadeOut, removeCB, NULL);
    label->runAction(sequence);
}
