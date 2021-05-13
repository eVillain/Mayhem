#ifndef ViewLayer_h
#define ViewLayer_h

#include "cocos2d.h"
#include "cocos-ext.h"

class ViewLayer
{
public:
    virtual cocos2d::Node* getRoot() = 0;
    virtual const std::string& getDescriptor() const = 0;
    virtual void update(const float deltaTime) = 0;
};

#endif /* ViewLayer_h */
