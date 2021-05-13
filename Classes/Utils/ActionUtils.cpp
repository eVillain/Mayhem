#include "ActionUtils.h"

namespace ActionUtils {

void fadeInElements(const std::vector<cocos2d::Node*>& nodes,
                    const float fadeTime,
                    const float interval)
{
    float currentDelay = 0.f;
    
    for (cocos2d::Node* node : nodes)
    {
        node->setOpacity(0);
        
        cocos2d::FadeIn* fadeIn = cocos2d::FadeIn::create(fadeTime);
        if (currentDelay == 0.f)
        {
            currentDelay += interval;
            node->runAction(fadeIn);
            continue;
        }
        
        node->setCascadeOpacityEnabled(true);
        
        cocos2d::DelayTime* delay = cocos2d::DelayTime::create(currentDelay);
        cocos2d::Sequence* sequence = cocos2d::Sequence::createWithTwoActions(delay, fadeIn);
        node->runAction(sequence);
        
        currentDelay += interval;
    }
}

};
