#ifndef LightController_h
#define LightController_h

#include "LightConstants.h"

class GameSettings;
class LightModel;
class AddLightEvent;
class RemoveLightEvent;

class LightController
{
public:
    static const std::string SETTING_RENDER_LIGHTING;

    LightController(std::shared_ptr<LightModel> model,
                    std::shared_ptr<GameSettings> gameSettings);
    ~LightController();
    
    void initialize();
    void shutdown();
    void enable();
    
    void update(const float deltaTime);

    cocos2d::RefPtr<cocos2d::RenderTexture> getOccluderTexture() const { return m_occluderTexture; }
    cocos2d::RefPtr<cocos2d::RenderTexture> getOccluderSliceTexture() const { return m_occluderSliceTexture; }
    cocos2d::RefPtr<cocos2d::RenderTexture> getDistortedTexture() const { return m_distortedTexture; }
    cocos2d::RefPtr<cocos2d::RenderTexture> getMinimumDistanceTexture() const { return m_minimumDistanceTexture; }
    cocos2d::RefPtr<cocos2d::RenderTexture> getShadowTexture() const { return m_shadowMapTexture; }
    cocos2d::RefPtr<cocos2d::RenderTexture> getLightMapTexture() const { return m_lightMapTexture; }

private:
    static const int MAX_LIGHT_RESOLUTION;
    static const cocos2d::Size LIGHT_TEXTURE_SIZE;
    static const cocos2d::Rect LIGHT_TEXTURE_RECT;

    std::shared_ptr<LightModel> m_model;
    std::shared_ptr<GameSettings> m_gameSettings;
    
    cocos2d::RefPtr<cocos2d::RenderTexture> m_occluderTexture; // Contains all shadow casting pixels in zoomed-out view
    cocos2d::RefPtr<cocos2d::RenderTexture> m_occluderSliceTexture; // Contains slice of shadow casters for current light
    cocos2d::RefPtr<cocos2d::RenderTexture> m_distortedTexture; // Contains light-perspective warped shadow caster distances in red and green channels
    cocos2d::RefPtr<cocos2d::RenderTexture> m_minimumDistanceTexture; // Contains minimum distance from light perspective to each shadow pixel (warped)
    cocos2d::RefPtr<cocos2d::RenderTexture> m_shadowMapTexture; // Contains light intensity with shadows carved out using minimum distance
    cocos2d::RefPtr<cocos2d::RenderTexture> m_lightMapTexture; // Contains all current lighting, color should be multiplied by game view contents
    
    cocos2d::RefPtr<cocos2d::GLProgram> m_computeDistanceShader;
    cocos2d::RefPtr<cocos2d::GLProgram> m_minimumDistanceShader;
    cocos2d::RefPtr<cocos2d::GLProgram> m_shadowMapNoBlurShader;
    cocos2d::RefPtr<cocos2d::GLProgram> m_lightShader;

    bool m_drawDebug;
    
    void renderStaticLights();
    void renderOccluders();
    void renderOccludersToShadowMap(const cocos2d::Rect& lightScreenIntersect,
                                    const cocos2d::Rect& lightScreenRect);
    void renderDistanceToLight(const cocos2d::Rect& lightScreenRect,
                               const cocos2d::Rect& lightScreenIntersect,
                               const cocos2d::Vec2& lightScreenPosition,
                               const float lightScreenRadius);
    void renderMinimumDistance(const cocos2d::Rect& lightScreenIntersect,
                               const cocos2d::Vec2& lightIntersectPosition,
                               const float lightScreenRadius);
    void renderShadows(const cocos2d::Color4F& lightColor,
                       const cocos2d::Vec2& lightScreenPosition,
                       const float lightScreenRadius,
                       const cocos2d::Rect& lightScreenRect,
                       const cocos2d::Rect& lightScreenIntersect);
    void renderLight(const cocos2d::Color4F& lightColor,
                     const float lightScreenRadius);
    void renderToTexture(cocos2d::Sprite* lightSprite,
                         cocos2d::RenderTexture* texture);

    void onAddLight(const AddLightEvent& event);
    void onRemoveLight(const RemoveLightEvent& event);
};

#endif /* LightController_h */
