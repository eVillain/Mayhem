#ifndef CameraController_h
#define CameraController_h

#include "cocos2d.h"

class CameraModel;

class CameraController
{
public:
    CameraController(std::shared_ptr<CameraModel> model);
    ~CameraController();
    
    void initialize();
    
    void update(const float deltaTime);
    
    cocos2d::Vec2 getViewPosition() const;
    cocos2d::Rect getViewRect() const;
private:
    std::shared_ptr<CameraModel> m_model;
    
    void updatePan(const float deltaTime);
    void updateZoom(const float deltaTime);
};

#endif /* Camera_h */
