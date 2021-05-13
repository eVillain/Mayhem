#include "EntityView.h"
#include "Core/Injector.h"
#include "GameView.h"

const int EntityView::ANIMATION_TAG = 555;
const size_t EntityView::HEAD_INDEX = 0;
const size_t EntityView::ARM_LEFT_INDEX = 1;
const size_t EntityView::ARM_RIGHT_INDEX = 2;
const size_t EntityView::WEAPON_INDEX = 3;
const cocos2d::Vec2 EntityView::HEAD_POS = cocos2d::Vec2(8, 12);
const cocos2d::Vec2 EntityView::ARM_L_POS = cocos2d::Vec2(11.f, 7.f);
const cocos2d::Vec2 EntityView::ARM_L_POS_FLIPPED = cocos2d::Vec2(5.f, 7.f);
const cocos2d::Vec2 EntityView::ARM_L_ANCHOR = cocos2d::Vec2(11.f / 16.f, 7.f / 16.f);
const cocos2d::Vec2 EntityView::ARM_L_ANCHOR_FLIPPED = cocos2d::Vec2(5.f / 16.f, 7.f / 16.f);
const cocos2d::Vec2 EntityView::ARM_R_POS = cocos2d::Vec2(5.f, 7.f);
const cocos2d::Vec2 EntityView::ARM_R_ANCHOR = cocos2d::Vec2(5.f / 16.f, 7.f / 16.f);

EntityView::EntityView(const uint16_t entityID, const EntityType type)
: m_entityID(entityID)
, m_type(type)
, m_sprite(nullptr)
{
}

void EntityView::setupAnimations(CharacterType character)
{
    std::string characterSprite;
    switch (character)
    {
        case Character_Base:
            characterSprite = "Base";
            break;
        default:
            break;
    }

    m_sprite = cocos2d::Sprite::createWithSpriteFrameName("TorsoIdle-0.png");
    m_sprite->setTag(m_entityID);
    
    auto headSprite = cocos2d::Sprite::createWithSpriteFrameName(characterSprite + "Head.png");
    headSprite->setPosition(HEAD_POS);
    m_sprite->addChild(headSprite);
    m_secondarySprites[HEAD_INDEX] = headSprite;
    
    auto armLSprite = cocos2d::Sprite::createWithSpriteFrameName("ArmLIdle-0.png");
    armLSprite->setAnchorPoint(ARM_L_ANCHOR);
    armLSprite->setPosition(ARM_L_POS);
    m_sprite->addChild(armLSprite, -2);
    m_secondarySprites[ARM_LEFT_INDEX] = armLSprite;
    
    auto armRSprite = cocos2d::Sprite::createWithSpriteFrameName("ArmRIdle-0.png");
    armRSprite->setAnchorPoint(ARM_R_ANCHOR);
    armRSprite->setPosition(ARM_R_POS);
    m_sprite->addChild(armRSprite, 2);
    m_secondarySprites[ARM_RIGHT_INDEX] = armRSprite;
    
    cocos2d::RefPtr<cocos2d::Animation> idle = createAnimation("TorsoIdle-", 1, 0.06f);
    m_animations[Idle] = cocos2d::RepeatForever::create(cocos2d::Animate::create(idle));
    m_animations[Idle]->setTag(ANIMATION_TAG);
    
    cocos2d::RefPtr<cocos2d::Animation> grab = createAnimation("TorsoGrab-", 2, 0.06f);
    m_animations[Grab] = cocos2d::RepeatForever::create(cocos2d::Animate::create(grab));
    m_animations[Grab]->setTag(ANIMATION_TAG);
    
    cocos2d::RefPtr<cocos2d::Animation> walk = createAnimation("TorsoWalk-", 8, 0.06f);
    auto walkRepeat = cocos2d::RepeatForever::create(cocos2d::Animate::create(walk));
    m_animations[Walk] = cocos2d::Speed::create(walkRepeat, 1.0f);
    m_animations[Walk]->setTag(ANIMATION_TAG);
    m_animations[Run] = cocos2d::Speed::create(walkRepeat, 2.0f);
    m_animations[Run]->setTag(ANIMATION_TAG);
    
    cocos2d::RefPtr<cocos2d::Animation> idleArmL = createAnimation("ArmLIdle-", 1, 0.06f);
    m_animations[IdleArmL] = cocos2d::RepeatForever::create(cocos2d::Animate::create(idleArmL));
    m_animations[IdleArmL]->setTag(ANIMATION_TAG);
    cocos2d::RefPtr<cocos2d::Animation> idleArmR = createAnimation("ArmRIdle-", 1, 0.06f);
    m_animations[IdleArmR] = cocos2d::RepeatForever::create(cocos2d::Animate::create(idleArmR));
    m_animations[IdleArmR]->setTag(ANIMATION_TAG);

    cocos2d::RefPtr<cocos2d::Animation> walkArmL = createAnimation("ArmLWalk-", 8, 0.06f);
    auto walkRepeatL = cocos2d::RepeatForever::create(cocos2d::Animate::create(walkArmL));
    m_animations[WalkArmL] = cocos2d::Speed::create(walkRepeatL, 1.0f);
    m_animations[WalkArmL]->setTag(ANIMATION_TAG);
    cocos2d::RefPtr<cocos2d::Animation> walkArmR = createAnimation("ArmRWalk-", 8, 0.06f);
    auto walkRepeatR = cocos2d::RepeatForever::create(cocos2d::Animate::create(walkArmR));
    m_animations[WalkArmR] = cocos2d::Speed::create(walkRepeatR, 1.0f);
    m_animations[WalkArmR]->setTag(ANIMATION_TAG);
    
    cocos2d::RefPtr<cocos2d::Animation> holdArmL = createAnimation("ArmLHold-", 1, 0.06f);
    m_animations[HoldArmL] = cocos2d::RepeatForever::create(cocos2d::Animate::create(holdArmL));
    m_animations[HoldArmL]->setTag(ANIMATION_TAG);
    cocos2d::RefPtr<cocos2d::Animation> holdArmR = createAnimation("ArmRHold-", 1, 0.06f);
    m_animations[HoldArmR] = cocos2d::RepeatForever::create(cocos2d::Animate::create(holdArmR));
    m_animations[HoldArmR]->setTag(ANIMATION_TAG);
    
    cocos2d::RefPtr<cocos2d::Animation> reloadArmR = createAnimation("ArmRReload-", 8, 0.06f);
    auto reloadRepeatR = cocos2d::RepeatForever::create(cocos2d::Animate::create(reloadArmR));
    m_animations[Reload] = cocos2d::Speed::create(reloadRepeatR, 1.0f);
    m_animations[Reload]->setTag(ANIMATION_TAG);

    cocos2d::RefPtr<cocos2d::Animation> idleFace = createAnimation("HeadBlink-", 3, 0.06f);
    cocos2d::DelayTime* idleFaceDelay = cocos2d::DelayTime::create(10.f);
    cocos2d::Sequence* idleFaceSeq = cocos2d::Sequence::create(idleFaceDelay, cocos2d::Animate::create(idleFace), cocos2d::Animate::create(idleFace)->reverse(), NULL);
    m_animations[IdleFace] = cocos2d::RepeatForever::create(idleFaceSeq);
    m_animations[IdleFace]->setTag(ANIMATION_TAG);
    
    cocos2d::RefPtr<cocos2d::Animation> aimFace = createAnimation("HeadWink-", 3, 0.06f);
    m_animations[AimFace] = cocos2d::Sequence::create(cocos2d::Animate::create(aimFace), cocos2d::DelayTime::create(60000.f),  NULL);
    m_animations[AimFace]->setTag(ANIMATION_TAG);
    
    cocos2d::RefPtr<cocos2d::Animation> unAimFace = createAnimation("HeadWink-", 3, 0.06f);
    m_animations[UnAimFace] = cocos2d::Sequence::create(cocos2d::Animate::create(unAimFace)->reverse(), cocos2d::DelayTime::create(1.f),  NULL);
    m_animations[UnAimFace]->setTag(ANIMATION_TAG);
}

void EntityView::removeAnimations()
{
    auto gameView = Injector::globalInjector().getInstance<GameView>();
    gameView->getSpriteBatch()->removeChild(m_sprite, true);
    m_sprite = nullptr;
    
    m_animations.clear();
}

void EntityView::runAnimation(const int animation)
{
    m_sprite->stopActionByTag(ANIMATION_TAG);
    m_sprite->runAction(m_animations[animation]);
}

const size_t EntityView::getAnimation(cocos2d::Action* action) const
{
    for (auto animation : m_animations)
    {
        if (animation.second == action)
        {
            return animation.first;
        }
    }
    return 0;
}

void EntityView::setupSprite(const std::string& frameName)
{
    if (!m_sprite)
    {
        m_sprite = createSprite(frameName);
    }
    else
    {
        m_sprite->setSpriteFrame(frameName);
    }
}

void EntityView::setupSecondarySprite(const std::string& frameName,
                                      const size_t index /* = 0*/)
{
    if (m_secondarySprites.size() <= index)
    {
        m_secondarySprites[index] = createSprite(frameName);
    }
    else
    {
        m_secondarySprites[index]->setSpriteFrame(frameName);
    }
}

void EntityView::removeSecondarySprite(const size_t index /* = 0*/)
{
    m_secondarySprites.erase(index);
}

void EntityView::setFlippedX(const bool flipX)
{
    m_sprite->setFlippedX(flipX);
    for (const auto& sprite : m_secondarySprites)
    {
        sprite.second->setFlippedX(flipX);
    }
}

cocos2d::RefPtr<cocos2d::Sprite> EntityView::createSprite(const std::string& frameName)
{
    cocos2d::RefPtr<cocos2d::Sprite> sprite = cocos2d::Sprite::createWithSpriteFrameName(frameName);
    return sprite;
}

cocos2d::RefPtr<cocos2d::Animation> EntityView::createAnimation(const std::string frameName,
                                                                const size_t numFrames,
                                                                const float frameDelay)
{
    cocos2d::SpriteFrameCache* cache = cocos2d::SpriteFrameCache::getInstance();
    
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames(numFrames);
    
    char str[100] = {0};
    for(size_t i = 0; i < numFrames; i++)
    {
        sprintf(str, "%s%i.png", frameName.c_str(), i);
        cocos2d::SpriteFrame* frame = cache->getSpriteFrameByName( str );
        animFrames.pushBack(frame);
    }
    
    cocos2d::RefPtr<cocos2d::Animation> animation = cocos2d::Animation::createWithSpriteFrames(animFrames, frameDelay);
    return animation;
}
