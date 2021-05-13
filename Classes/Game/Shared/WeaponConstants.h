#ifndef WeaponConstants_h
#define WeaponConstants_h

#include "cocos2d.h"

namespace WeaponConstants
{
    struct WeaponStateData
    {
        cocos2d::Vec2 weaponPosition;
        cocos2d::Vec2 projectilePosition;
        float weaponAngle;
        float projectileAngle;
    };

    static WeaponStateData getWeaponData(const cocos2d::Vec2& playerPosition,
                                         const cocos2d::Vec2& aimPosition,
                                         const cocos2d::Vec2& holdOffset,
                                         const cocos2d::Vec2 projectileOffset,
                                         const bool flipX)
    {
        WeaponStateData output;
        
        const cocos2d::Vec2 shoulder = cocos2d::Vec2(-3.f, -1.f);
        cocos2d::Vec2 hOffset = holdOffset;
        cocos2d::Vec2 pOffset = projectileOffset;
        
        if (flipX)
        {
            hOffset.x *= -1.f;
            hOffset.x -= 1.f;
            pOffset.x *= -1.f;
            pOffset.x += -1.f;
        }
        
        const cocos2d::Vec2 shoulderPosition = playerPosition + shoulder;
        const cocos2d::Vec2 shoulderToProjectile = pOffset - shoulder;
        const cocos2d::Vec2 shoulderToTarget = aimPosition - shoulderPosition;
        const float shoulderTargetAngle = shoulderToTarget.getAngle();
        
        const cocos2d::Vec2 aimOffset = cocos2d::Vec2(0, (shoulderToProjectile.y - 1.f) * (flipX ? -1.f : 1.f)).rotateByAngle(cocos2d::Vec2::ZERO, shoulderTargetAngle);
        const cocos2d::Vec2 aimVector = (aimPosition - aimOffset) - shoulderPosition;
        
        const float aimAngleR = aimVector.getAngle() + (flipX ? M_PI : 0.f);
        output.projectileAngle = -aimVector.getAngle();
        output.weaponAngle = aimAngleR * (-180.f / M_PI);
        output.weaponPosition = hOffset.rotateByAngle(cocos2d::Vec2::ZERO, aimAngleR);
        output.projectilePosition = shoulderPosition + pOffset.rotateByAngle(cocos2d::Vec2::ZERO, aimAngleR);
        
        return output;
    };
    

}

#endif /* WeaponConstants_h */
