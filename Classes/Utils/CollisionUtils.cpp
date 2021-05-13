#include "CollisionUtils.h"
#include <limits>

bool CollisionUtils::pointToCircle(const cocos2d::Vec2& point,
                                   const cocos2d::Vec2& circle,
                                   const float radius)
{
    float distance = (point - circle).length();
    return distance <= radius;
}

bool CollisionUtils::lineToPoint(const cocos2d::Vec2& lineStart,
                                 const cocos2d::Vec2& lineEnd,
                                 const cocos2d::Vec2& point)
{
    
    // get distance from the point to the two ends of the line
    float d1 = (point - lineStart).length();
    float d2 = (point - lineEnd).length();
    
    // get the length of the line
    float lineLen = (lineStart - lineEnd).length();
    
    // since floats are so minutely accurate, add
    // a little buffer zone that will give collision
    float buffer = 0.01;    // higher # = less accurate
    
    // if the two distances are equal to the line's
    // length, the point is on the line!
    // note we use the buffer here to give a range,
    // rather than one #
    
    return (d1 + d2 >= lineLen - buffer &&
            d1 + d2 <= lineLen + buffer);
}

bool CollisionUtils::lineToCircle(const cocos2d::Vec2& lineStart,
                                const cocos2d::Vec2& lineEnd,
                                const cocos2d::Vec2& circle,
                                const float radius)
{
    
    // is either end INSIDE the circle?
    // if so, return true immediately
    bool inside1 = pointToCircle(lineStart, circle, radius);
    bool inside2 = pointToCircle(lineEnd, circle, radius);
    if (inside1 || inside2) return true;
    
    // get length of the line
    float lineLen = (lineStart - lineEnd).length();
    
    // get dot product of the line and circle
    float dot = (((circle.x-lineStart.x)*(lineEnd.x-lineStart.x)) + ((circle.y-lineStart.y)*(lineEnd.y-lineStart.y))) / pow(lineLen, 2);
    
    // find the closest point on the line
    cocos2d::Vec2 closest = cocos2d::Vec2(lineStart.x + (dot * (lineEnd.x-lineStart.x)), lineStart.y + (dot * (lineEnd.y-lineStart.y)));
    
    // is this point actually on the line segment?
    // if so keep going, but if not, return false
    bool onSegment = lineToPoint(lineStart,lineEnd, closest);
    if (!onSegment) return false;
    
    // get distance to closest point
    float distance = (closest - circle).length();
    
    if (distance <= radius)
    {
        return true;
    }
    
    return false;
}



bool CollisionUtils::lineToLine(const cocos2d::Vec2& aStart,
                                 const cocos2d::Vec2& aEnd,
                                 const cocos2d::Vec2& bStart,
                                 const cocos2d::Vec2& bEnd,
                                 cocos2d::Vec2& output)
{
    float s02_x, s02_y, s10_x, s10_y, s32_x, s32_y, s_numer, t_numer, denom, t;
    s10_x = aEnd.x - aStart.x;
    s10_y = aEnd.y - aStart.y;
    s32_x = bEnd.x - bStart.x;
    s32_y = bEnd.y - bStart.y;

    denom = s10_x * s32_y - s32_x * s10_y;
    if (denom == 0)
        return false; // Collinear
    bool denomPositive = denom > 0;

    s02_x = aStart.x - bStart.x;
    s02_y = aStart.y - bStart.y;
    s_numer = s10_x * s02_y - s10_y * s02_x;
    if ((s_numer < 0) == denomPositive)
        return false; // No collision

    t_numer = s32_x * s02_y - s32_y * s02_x;
    if ((t_numer < 0) == denomPositive)
        return false; // No collision

    if (((s_numer > denom) == denomPositive) || ((t_numer > denom) == denomPositive))
        return false; // No collision
    // Collision detected
    t = t_numer / denom;

    output.x = aStart.x + (t * s10_x);
    output.y = aStart.y + (t * s10_y);

    return true;
}

bool CollisionUtils::lineToRect(const cocos2d::Vec2& lineStart,
                                const cocos2d::Vec2& lineEnd,
                                const cocos2d::Rect& rect,
                                cocos2d::Vec2& output)
{
    const cocos2d::Vec2 bottomLeft = cocos2d::Vec2(rect.origin.x, rect.origin.y);
    const cocos2d::Vec2 topLeft = cocos2d::Vec2(rect.origin.x, rect.origin.y + rect.size.height);
    const cocos2d::Vec2 topRight = cocos2d::Vec2(rect.origin.x + rect.size.width, rect.origin.y + rect.size.height);
    const cocos2d::Vec2 bottomRight = cocos2d::Vec2(rect.origin.x + rect.size.width, rect.origin.y);
    
    bool didIntersect = false;
    cocos2d::Vec2 intersection;
    float dist = INFINITY;
    if (lineToLine(lineStart, lineEnd, bottomLeft, topLeft, intersection)) // Left side of box
    {
        dist = (intersection - lineStart).lengthSquared();
        output = intersection;
        didIntersect = true;
    }
    if (lineToLine(lineStart, lineEnd, bottomRight, topRight, intersection)) // Right side of box
    {
        float newDist = (intersection - lineStart).lengthSquared();
        if (newDist < dist)
        {
            dist = newDist;
            output = intersection;
        }
        didIntersect = true;
    }

    if (lineToLine(lineStart, lineEnd, topLeft, topRight, intersection)) // Top side of box
    {
        float newDist = (intersection - lineStart).lengthSquared();
        if (newDist < dist)
        {
            dist = newDist;
            output = intersection;
        }
        didIntersect = true;
    }
    if (lineToLine(lineStart, lineEnd, bottomLeft, bottomRight, intersection))  // Bottom side of box
    {
        float newDist = (intersection - lineStart).lengthSquared();
        if (newDist < dist)
        {
            dist = newDist;
            output = intersection;
        }
        didIntersect = true;
    }
    
    return didIntersect;
}

const cocos2d::Rect CollisionUtils::minkowskiDifference(const cocos2d::Rect& rectA,
                                                        const cocos2d::Rect& rectB)
{
    return cocos2d::Rect(rectB.getMinX() - rectA.getMaxX(),
                         rectB.getMinY() - rectA.getMaxY(),
                         rectA.size.width + rectB.size.width,
                         rectA.size.height + rectB.size.height);
}

const cocos2d::Vec2 CollisionUtils::closestPointOnBoundsToPoint(const cocos2d::Vec2& point,
                                                                const cocos2d::Rect& rect)
{
    const float minX = rect.getMinX();
    const float maxX = rect.getMaxX();
    const float minY = rect.getMinY();
    const float maxY = rect.getMaxY();
    float minDist = std::abs(point.x - minX);
    cocos2d::Vec2 boundsPoint = cocos2d::Vec2(minX, point.y);
    
    if (std::abs(point.x - maxX) < minDist)
    {
        minDist = std::abs(maxX - point.x);
        boundsPoint = cocos2d::Vec2(maxX, point.y);
    }
    if (std::abs(maxY - point.y) < minDist)
    {
        minDist = std::abs(maxY - point.y);
        boundsPoint = cocos2d::Vec2(point.x, maxY);
    }
    if (std::abs(point.y - minY) < minDist)
    {
        boundsPoint = cocos2d::Vec2(point.x, minY);
    }
    
    return boundsPoint;
}

const float CollisionUtils::getRayIntersectionFractionOfFirstRay(const cocos2d::Vec2& originA,
                                                                 const cocos2d::Vec2& endA,
                                                                 const cocos2d::Vec2& originB,
                                                                 const cocos2d::Vec2& endB)
{
    const cocos2d::Vec2 r = endA - originA;
    const cocos2d::Vec2 s = endB - originB;
    
    float numerator = (originB - originA).cross(r);
    float denominator = r.cross(s);
    
    if (numerator == 0 && denominator == 0)
    {
        // the lines are co-linear
        // check if they overlap
        // todo: calculate intersection point
        return std::numeric_limits<float>::max();
    }
    if (denominator == 0)
    {
        // lines are parallel
        return std::numeric_limits<float>::max();
    }
    
    float u = numerator / denominator;
    float t = ((originB - originA).cross(s)) / denominator;
    if ((t >= 0) && (t <= 1) && (u >= 0) && (u <= 1))
    {
        return t;
    }
    
    return std::numeric_limits<float>::max();
}

const float CollisionUtils::getRayIntersectionFraction(const cocos2d::Vec2& origin,
                                                       const cocos2d::Vec2& direction,
                                                       const cocos2d::Rect& rect)
{
    const cocos2d::Vec2 min = rect.origin + cocos2d::Vec2::ONE;
    const cocos2d::Vec2 max = min + rect.size - cocos2d::Vec2(2,2);
    const cocos2d::Vec2 end = origin + direction;
    
    // for each of the AABB's four edges
    // calculate the minimum fraction of "direction"
    // in order to find where the ray FIRST intersects
    // the AABB (if it ever does)
    
    // left
    float minT = getRayIntersectionFractionOfFirstRay(origin, end, cocos2d::Vec2(min.x, min.y), cocos2d::Vec2(min.x, max.y));
    
    float x;
    x = getRayIntersectionFractionOfFirstRay(origin, end, cocos2d::Vec2(min.x, max.y), cocos2d::Vec2(max.x, max.y));
    if (x < minT)
    {
        minT = x; // top
    }
    x = getRayIntersectionFractionOfFirstRay(origin, end, cocos2d::Vec2(max.x, max.y), cocos2d::Vec2(max.x, min.y));
    if (x < minT)
    {
        minT = x;// right

    }
    x = getRayIntersectionFractionOfFirstRay(origin, end, cocos2d::Vec2(max.x, min.y), cocos2d::Vec2(min.x, min.y));
    if (x < minT)
    {
        minT = x; // bottom
    }
    // ok, now we should have found the fractional component along the ray where we collided
    return minT;
}
