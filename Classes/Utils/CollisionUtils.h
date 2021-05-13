#ifndef COLLISION_UTILS_H
#define COLLISION_UTILS_H

#include "cocos2d.h"

class CollisionUtils
{
public:
    static bool pointToCircle(const cocos2d::Vec2& point,
                              const cocos2d::Vec2& circle,
                              const float radius);
  
    static bool lineToPoint(const cocos2d::Vec2& lineStart,
                            const cocos2d::Vec2& lineEnd,
                            const cocos2d::Vec2& point);
    
    static bool lineToCircle(const cocos2d::Vec2& lineStart,
                             const cocos2d::Vec2& lineEnd,
                             const cocos2d::Vec2& circle,
                             const float radius);
    
    static bool lineToLine(const cocos2d::Vec2& aStart,
                           const cocos2d::Vec2& aEnd,
                           const cocos2d::Vec2& bStart,
                           const cocos2d::Vec2& bEnd,
                           cocos2d::Vec2& output);
    
    static bool lineToRect(const cocos2d::Vec2& lineStart,
                           const cocos2d::Vec2& lineEnd,
                           const cocos2d::Rect& rect,
                           cocos2d::Vec2& output);

    static const cocos2d::Rect minkowskiDifference(const cocos2d::Rect& rectA,
                                                   const cocos2d::Rect& rectB);

    static const cocos2d::Vec2 closestPointOnBoundsToPoint(const cocos2d::Vec2& point,
                                                           const cocos2d::Rect& rect);
    
    // taken from https://github.com/pgkelley4/line-segments-intersect/blob/master/js/line-segments-intersect.js
    // which was adapted from http://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
    // returns the point where they intersect (if they intersect)
    // returns std::numeric_limits<float>::max() if they don't intersect
    static const float getRayIntersectionFractionOfFirstRay(const cocos2d::Vec2& originA,
                                                            const cocos2d::Vec2& endA,
                                                            const cocos2d::Vec2& originB,
                                                            const cocos2d::Vec2& endB);
    
    static const float getRayIntersectionFraction(const cocos2d::Vec2& origin,
                                                  const cocos2d::Vec2& direction,
                                                  const cocos2d::Rect& rect);
};


#endif /* COLLISION_UTILS_H */
