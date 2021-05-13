#include "CrosshairView.h"

const std::string CrosshairView::CROSSHAIR_FRAME_DEFAULT = "Crosshair-1.png";
const std::string CrosshairView::CROSSHAIR_FRAME_SHRINK1 = "Crosshair-2.png";
const std::string CrosshairView::CROSSHAIR_FRAME_SHRINK2 = "Crosshair-3.png";
const std::string CrosshairView::CROSSHAIR_FRAME_SHOT = "Crosshair-4.png";
const std::string CrosshairView::CROSSHAIR_FRAME_HITMARK = "Crosshair-5.png";

CrosshairView::CrosshairView()
: m_crosshair(nullptr)
, m_shrinkAnimation(nullptr)
, m_mode(CrosshairMode::WIDE)
{

}

void CrosshairView::initialize()
{
    m_crosshair = cocos2d::Sprite::createWithSpriteFrameName(CROSSHAIR_FRAME_DEFAULT);
    m_crosshair->setAnchorPoint(cocos2d::Vec2(0.5,0.5));
    
    cocos2d::SpriteFrameCache* cache = cocos2d::SpriteFrameCache::getInstance();
    
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames(2);
    cocos2d::SpriteFrame* frame1 = cache->getSpriteFrameByName(CROSSHAIR_FRAME_SHRINK1);
    animFrames.pushBack(frame1);
    cocos2d::SpriteFrame* frame2 = cache->getSpriteFrameByName(CROSSHAIR_FRAME_SHRINK2);
    animFrames.pushBack(frame2);
    
    auto shrinkAnim = cocos2d::Animation::createWithSpriteFrames(animFrames, 0.06f);
    m_shrinkAnimation = cocos2d::Animate::create(shrinkAnim);
}

void CrosshairView::setMode(const CrosshairMode mode)
{
    if (m_mode == mode)
    {
        return;
    }
    m_mode = mode;
    m_crosshair->stopAllActions();
    
    switch (m_mode) {
        case WIDE:
            m_crosshair->setSpriteFrame(CROSSHAIR_FRAME_DEFAULT);
            break;
        case NARROW:
            m_crosshair->runAction(m_shrinkAnimation);
            break;
        case SHOT:
            m_crosshair->setSpriteFrame(CROSSHAIR_FRAME_SHOT);
            break;
        case HITMARK:
            m_crosshair->setSpriteFrame(CROSSHAIR_FRAME_HITMARK);
            break;

        default:
            break;
    }
}
