#ifndef FOLLOW_NODE_H
#define FOLLOW_NODE_H

#include "cocos2d.h"
#include <vector>

class FollowNode : public cocos2d::Action
{
public:
    /**
     * Creates the action with a set offset or with no offset.
     *
     * @param followedNode  The node to be followed.
     * @param offset  The offset.
     */
    
    static FollowNode* create(cocos2d::Node *followedNode, const cocos2d::Vec2& offset = cocos2d::Vec2::ZERO);

    /**
     * @param dt in seconds.
     */
    virtual void step(float dt) override;
    virtual bool isDone() const override;
    virtual void stop() override;
    
    FollowNode()
    : _followedNode(nullptr)
    , _offset(cocos2d::Vec2::ZERO)
    {}

    virtual ~FollowNode();
    
    /**
     * Initializes the action with a set offset or with no offset.
     *
     * @param followedNode  The node to be followed.
     * @param offset  The offset.
     */
    bool initWithTarget(cocos2d::Node *followedNode, const cocos2d::Vec2& offset = cocos2d::Vec2::ZERO);
    
protected:
    /** Node to follow. */
    cocos2d::Node *_followedNode;
    
    /** Offset value. */
    cocos2d::Vec2 _offset;
};

#endif /* FOLLOW_NODE_H */
