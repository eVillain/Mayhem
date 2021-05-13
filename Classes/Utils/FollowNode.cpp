#include "FollowNode.h"

FollowNode::~FollowNode()
{
    CC_SAFE_RELEASE(_followedNode);
}

FollowNode* FollowNode::create(cocos2d::Node *followedNode, const cocos2d::Vec2& offset/* = cocos2d::Vec2::ZERO*/)
{
    FollowNode *follow = new (std::nothrow) FollowNode();
    
    bool valid;
    
    valid = follow->initWithTarget(followedNode, offset);
    
    if (follow && valid)
    {
        follow->autorelease();
        return follow;
    }
    
    delete follow;
    return nullptr;
}

bool FollowNode::initWithTarget(cocos2d::Node *followedNode, const cocos2d::Vec2& offset)
{
    CCASSERT(followedNode != nullptr, "FollowedNode can't be NULL");
    if(followedNode == nullptr)
    {
        CCLOG("Follow::initWithTarget error: followedNode is nullptr!");
        return false;
    }
    
    followedNode->retain();
    _followedNode = followedNode;
    _offset = offset;
    
    return true;
}

void FollowNode::step(float dt)
{
    CC_UNUSED_PARAM(dt);
    
    _target->setPosition(_followedNode->getPosition() + _offset);
}

bool FollowNode::isDone() const
{
    return ( !_followedNode->isRunning() );
}

void FollowNode::stop()
{
    _target = nullptr;
    Action::stop();
}
