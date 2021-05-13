#ifndef CameraModel_h
#define CameraModel_h

#include "cocos2d.h"

class CameraModel
{
public:
    static const float DEFAULT_ZOOM_LEVEL;
    
    CameraModel();
    
    void setPosition(cocos2d::Vec2 position) { m_position = position; }
    cocos2d::Vec2 getPosition() { return m_position; }

    void setTargetPosition(cocos2d::Vec2 position) { m_targetPosition = position; }
    cocos2d::Vec2 getTargetPosition() { return m_targetPosition; }
    
    void setTargetDeltaAccumulator(cocos2d::Vec2 delta) { m_targetDeltaAccumulator = delta; }
    cocos2d::Vec2 getTargetDeltaAccumulator() { return m_targetDeltaAccumulator; }
    
    void setZoom(float zoom) { m_zoom = zoom; }
    float getZoom() { return m_zoom; }
    
    void setTargetZoom(float targetZoom) { m_targetZoom = targetZoom; }
    float getTargetZoom() { return m_targetZoom; }

    void setMinZoom(float minZoom) { m_minZoom = minZoom; }
    float getMinZoom() { return m_minZoom; }

    void setMaxZoom(float maxZoom) { m_maxZoom = maxZoom; }
    float getMaxZoom() { return m_maxZoom; }
    
    void setElasticPan(bool elastic) { m_elasticPan = elastic; }
    bool getElasticPan() { return m_elasticPan; }
    
    void setElasticZoom(bool elastic) { m_elasticZoom = elastic; }
    bool getElasticZoom() { return m_elasticZoom; }
    
    void setPanElasticity(float elasticity) { m_panElasticity = elasticity; }
    float getPanElasticity() { return m_panElasticity; }
    
    void setZoomElasticity(float elasticity) { m_zoomElasticity = elasticity; }
    float getZoomElasticity() { return m_zoomElasticity; }

    void setMaxDistance(float maxDistance) { m_maxDistance = maxDistance; }
    float getMaxDistance() { return m_maxDistance; }
    
    void setPixelPerfect(bool pixelPerfect) { m_pixelPerfect = pixelPerfect; }
    bool getPixelPerfet() { return m_pixelPerfect; }
    
    void setScreenShake(const cocos2d::Vec2& screenShake) { m_screenShake = screenShake; }
    cocos2d::Vec2 getScreenShake() const { return m_screenShake; }
    float getShakeDampenFactor() const { return m_shakeDampenFactor; }
    bool getEnableScreenShake() const { return m_enableScreenShake; }

    void setCameraFollowPlayerID(uint8_t playerID) { m_cameraFollowPlayerID = playerID; }
    uint8_t getCameraFollowPlayerID() const { return m_cameraFollowPlayerID; }
    void setFollowPlayer(bool followPlayer) { m_followPlayer = followPlayer; }
    bool getFollowPlayer() const { return m_followPlayer; }
    
private:
    cocos2d::Vec2 m_position;
    cocos2d::Vec2 m_targetPosition;
    cocos2d::Vec2 m_targetDeltaAccumulator;
    cocos2d::Vec2 m_screenShake;

    float m_zoom;
    float m_targetZoom;
    float m_maxZoom;
    float m_minZoom;
    float m_panElasticity;
    float m_zoomElasticity;
    float m_maxDistance;
    float m_shakeDampenFactor;
    bool m_elasticPan;
    bool m_elasticZoom;
    bool m_pixelPerfect;
    bool m_enableScreenShake;
    
    uint8_t m_cameraFollowPlayerID;
    bool m_followPlayer;
};

#endif /* CameraModel_h */
