#ifndef CrosshairView_h
#define CrosshairView_h

#include "cocos2d.h"

class CrosshairView
{
public:
    enum CrosshairMode {
        WIDE,
        NARROW,
        SHOT,
        HITMARK
    };
    
    CrosshairView();
    
    void initialize();
    
    void setMode(const CrosshairMode mode);
    
    const cocos2d::RefPtr<cocos2d::Sprite>& getCrossHair() { return m_crosshair; }
    
private:
    cocos2d::RefPtr<cocos2d::Sprite> m_crosshair;
    cocos2d::RefPtr<cocos2d::Animate> m_shrinkAnimation;
    CrosshairMode m_mode;
    
    static const std::string CROSSHAIR_FRAME_DEFAULT;
    static const std::string CROSSHAIR_FRAME_SHRINK1;
    static const std::string CROSSHAIR_FRAME_SHRINK2;
    static const std::string CROSSHAIR_FRAME_SHOT;
    static const std::string CROSSHAIR_FRAME_HITMARK;
};

#endif /* CrosshairView_h */
