#include "LightController.h"
#include "LightModel.h"
#include "AddLightEvent.h"
#include "RemoveLightEvent.h"
#include "Core/Dispatcher.h"
#include "Core/Injector.h"
#include "GameView.h"
#include "GameSettings.h"
#include "cocos2d.h"

const std::string LightController::SETTING_RENDER_LIGHTING = "RenderLighting";
const int LightController::MAX_LIGHT_RESOLUTION = 512;
const cocos2d::Size LightController::LIGHT_TEXTURE_SIZE = cocos2d::Size(MAX_LIGHT_RESOLUTION, MAX_LIGHT_RESOLUTION);
const cocos2d::Rect LightController::LIGHT_TEXTURE_RECT = cocos2d::Rect(cocos2d::Vec2::ZERO, LIGHT_TEXTURE_SIZE);

LightController::LightController(std::shared_ptr<LightModel> model,
                                 std::shared_ptr<GameSettings> gameSettings)
: m_model(model)
, m_gameSettings(gameSettings)
, m_occluderTexture(nullptr)
, m_occluderSliceTexture(nullptr)
, m_distortedTexture(nullptr)
, m_shadowMapTexture(nullptr)
, m_minimumDistanceTexture(nullptr)
, m_computeDistanceShader(nullptr)
, m_minimumDistanceShader(nullptr)
, m_shadowMapNoBlurShader(nullptr)
, m_lightShader(nullptr)
{
}

void LightController::initialize()
{
    Dispatcher::globalDispatcher().addListener(AddLightEvent::descriptor, std::bind(&LightController::onAddLight, this, std::placeholders::_1));
    Dispatcher::globalDispatcher().addListener(RemoveLightEvent::descriptor, std::bind(&LightController::onRemoveLight, this, std::placeholders::_1));

    const cocos2d::Value& renderLightingSetting = m_gameSettings->getValue(SETTING_RENDER_LIGHTING, cocos2d::Value(true));
    if (renderLightingSetting.asBool())
    {
        enable();
    }
}

void LightController::shutdown()
{
    Dispatcher::globalDispatcher().removeListeners(AddLightEvent::descriptor);
    Dispatcher::globalDispatcher().removeListeners(RemoveLightEvent::descriptor);
    
    m_occluderTexture = nullptr;
    m_occluderSliceTexture = nullptr;
    m_distortedTexture = nullptr;
    m_shadowMapTexture = nullptr;
    m_minimumDistanceTexture = nullptr;
    m_computeDistanceShader = nullptr;
    m_minimumDistanceShader = nullptr;
    m_shadowMapNoBlurShader = nullptr;
    m_lightShader = nullptr;
}

void LightController::enable()
{
    // Note: this could be optimized to only use one color channel, however
    // Cocos2d does not allow for 8-bit RenderTextures, only RGB and RGBA are supported
    
    const cocos2d::Size winSize = cocos2d::Director::getInstance()->getWinSize();
    m_occluderTexture = cocos2d::RenderTexture::create(winSize.width, winSize.height,
                                                       cocos2d::Texture2D::PixelFormat::RGBA8888,
                                                       0);
    m_occluderTexture->getSprite()->setAnchorPoint(cocos2d::Vec2::ZERO);
    
    m_occluderSliceTexture = cocos2d::RenderTexture::create(MAX_LIGHT_RESOLUTION, MAX_LIGHT_RESOLUTION,
                                                            cocos2d::Texture2D::PixelFormat::RGBA8888,
                                                            0);
    m_occluderSliceTexture->getSprite()->setAnchorPoint(cocos2d::Vec2::ZERO);

    m_distortedTexture = cocos2d::RenderTexture::create(MAX_LIGHT_RESOLUTION, MAX_LIGHT_RESOLUTION,
                                                        cocos2d::Texture2D::PixelFormat::RGB888,
                                                        0);
    m_distortedTexture->getSprite()->setAnchorPoint(cocos2d::Vec2::ZERO);

    m_minimumDistanceTexture = cocos2d::RenderTexture::create(2, MAX_LIGHT_RESOLUTION,
                                                              cocos2d::Texture2D::PixelFormat::RGB888,
                                                              0);
    m_minimumDistanceTexture->getSprite()->setAnchorPoint(cocos2d::Vec2::ZERO);
    m_minimumDistanceTexture->getSprite()->setPosition(cocos2d::Vec2(20, 20));
    m_minimumDistanceTexture->getSprite()->setRotation(90.f);
    m_minimumDistanceTexture->getSprite()->setFlippedX(true);

    m_shadowMapTexture = cocos2d::RenderTexture::create(MAX_LIGHT_RESOLUTION, MAX_LIGHT_RESOLUTION,
                                                        cocos2d::Texture2D::PixelFormat::RGB888,
                                                        0);
    m_shadowMapTexture->getSprite()->setAnchorPoint(cocos2d::Vec2::ZERO);

    m_lightMapTexture = cocos2d::RenderTexture::create(winSize.width, winSize.height,
                                                       cocos2d::Texture2D::PixelFormat::RGB888,
                                                       0);
    
//    m_lightMapTexture->getSprite()->setBlendFunc(cocos2d::BlendFunc::ADDITIVE); // Additive lighting
    m_lightMapTexture->getSprite()->setBlendFunc({GL_ZERO, GL_SRC_COLOR}); // Mutiply lighting

    m_computeDistanceShader = cocos2d::GLProgram::createWithFilenames("res/shaders/vertex.vsh", "res/shaders/compute_distance.fsh");
    m_minimumDistanceShader = cocos2d::GLProgram::createWithFilenames("res/shaders/vertex.vsh", "res/shaders/minimum_distance.fsh");
    m_shadowMapNoBlurShader = cocos2d::GLProgram::createWithFilenames("res/shaders/vertex.vsh", "res/shaders/shadow_map_noblur.fsh");
    m_lightShader = cocos2d::GLProgram::createWithFilenames("res/shaders/vertex.vsh", "res/shaders/light_noshadow.fsh");
}

void LightController::renderStaticLights()
{
    auto director = cocos2d::Director::getInstance();
    const auto& gameView = Injector::globalInjector().getInstance<GameView>();
    auto staticLightNode = gameView->getStaticLightingNode();
    if (!staticLightNode)
    {
        return;
    }
    staticLightNode->setBlendFunc({GL_ONE, GL_ONE});
    staticLightNode->setPosition(gameView->getView()->getPosition());
    staticLightNode->setVisible(true);
    m_lightMapTexture->begin();
    ((cocos2d::Node*)staticLightNode)->visit();
    m_lightMapTexture->end();
    director->getRenderer()->render();
    staticLightNode->setVisible(false);
}

void LightController::update(const float deltaTime)
{
    const cocos2d::Color4F& ambience = m_model->getAmbienceColor();
    m_lightMapTexture->clear(ambience.r, ambience.g, ambience.b, ambience.a);

    renderStaticLights();
    
    std::map<size_t, LightData>& lights = m_model->getLights();
    if (lights.empty())
    {
        return;
    }
    
    const std::shared_ptr<GameView>& gameView = Injector::globalInjector().getInstance<GameView>();
    cocos2d::Director* director = cocos2d::Director::getInstance();
    
    const cocos2d::Size winSize = director->getWinSize();
    const cocos2d::Vec2 midWindow = winSize * 0.5f;
    cocos2d::Vec2 viewPos = gameView->getView()->getPosition();
    const cocos2d::Vec2 viewCenter = midWindow - viewPos;
    const cocos2d::Rect viewScreenRect = cocos2d::Rect(cocos2d::Vec2(0,0), winSize);
    const float pixelScale = gameView->getRenderTexture()->getSprite()->getScale();
    const cocos2d::Rect screenRect = cocos2d::Rect(viewCenter - (midWindow / pixelScale), winSize / pixelScale);
    
#if 1
    viewPos.x = std::round(viewPos.x);
    viewPos.y = std::round(viewPos.y);
#endif
    if (m_model->getDrawShadows())
    {
        renderOccluders();
    }
    else
    {
        renderLight(cocos2d::Color4F::WHITE, MAX_LIGHT_RESOLUTION * 0.5f);
    }
    
    std::vector<size_t> deadLightIDs;
    for (auto& lightPair : lights)
    {
        if (lightPair.second.lifeTime != -1.f)
        {
            lightPair.second.lifeTime -= deltaTime;
            if (lightPair.second.lifeTime <= 0.f) // Lifetime expired, remove
            {
                deadLightIDs.push_back(lightPair.first);
                continue;
            }
        }
        const LightData& light = lightPair.second;
        cocos2d::Vec2 lightPosition = light.position;
#if 1
            lightPosition.x = std::round(lightPosition.x);
            lightPosition.y = std::round(lightPosition.y);
#endif
        const float lightRadius = light.radius;
        const cocos2d::Size lightRadiusSize = cocos2d::Size(lightRadius, lightRadius);
        const cocos2d::Rect lightRect = cocos2d::Rect(lightPosition - lightRadiusSize, lightRadiusSize * 2.f);

        bool isOnScreen = lightRect.intersectsRect(screenRect);
        if (!isOnScreen)
        {
            continue;
        }
        const cocos2d::Vec2 lightScreenPosition = viewPos + lightPosition;
        const cocos2d::Rect lightScreenRect = cocos2d::Rect(lightScreenPosition - lightRadiusSize, lightRadiusSize * 2.f);
        
        const cocos2d::Vec2 origin = cocos2d::Vec2(std::max(lightScreenRect.origin.x, 0.f),
                                                   std::max(lightScreenRect.origin.y, 0.f));
        const cocos2d::Size size = cocos2d::Size(std::min(lightScreenRect.getMaxX() - origin.x, winSize.width - origin.x),
                                                 std::min(lightScreenRect.getMaxY() - origin.y, winSize.height - origin.y));
        const cocos2d::Rect lightScreenIntersect = cocos2d::Rect(origin, size);
        const cocos2d::Vec2 lightIntersectDiff = lightScreenIntersect.origin - lightScreenRect.origin;
        const cocos2d::Vec2 lightIntersectPosition = cocos2d::Vec2(lightRadius, lightRadius) - lightIntersectDiff;
        const cocos2d::Size lightSize = lightRadiusSize * 2.f;
        const cocos2d::Vec2 lightRenderPosition = (lightScreenPosition - lightRadiusSize) * 0.5f;
        
        if (m_model->getDrawShadows())
        {
            renderOccludersToShadowMap(lightScreenIntersect, lightScreenRect);
            
            renderDistanceToLight(lightScreenRect, lightScreenIntersect, lightScreenPosition, lightRadius);

            renderMinimumDistance(lightScreenIntersect, lightIntersectPosition, lightRadius);

            renderShadows(light.color, lightRenderPosition, lightRadius, lightScreenRect, lightScreenIntersect);

            bool clipTop = lightScreenRect.size.height > lightScreenIntersect.size.height && lightScreenRect.origin.y < lightScreenIntersect.origin.y;
            bool clipRight = lightScreenRect.size.width > lightScreenIntersect.size.width && lightScreenRect.origin.x < lightScreenIntersect.origin.x;
            const cocos2d::Vec2 relativeOccluderPos = cocos2d::Vec2(clipRight ? lightScreenIntersect.origin.x : lightRenderPosition.x,
                                                                    clipTop ? lightScreenIntersect.origin.y : lightRenderPosition.y);
            
            auto lightSprite = m_shadowMapTexture->getSprite();
            lightSprite->setBlendFunc(cocos2d::BlendFunc::ADDITIVE);
            lightSprite->setPosition(relativeOccluderPos);
            lightSprite->setTextureRect(cocos2d::Rect(cocos2d::Vec2::ZERO, lightScreenIntersect.size));
            lightSprite->setGLProgram(cocos2d::GLProgramCache::getInstance()->getGLProgram(cocos2d::GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR));
            renderToTexture(m_shadowMapTexture->getSprite(), m_lightMapTexture);
        }
        else
        {
            const cocos2d::Size staticLightSize = cocos2d::Size(MAX_LIGHT_RESOLUTION, MAX_LIGHT_RESOLUTION);
            // Add rendered light texture to lightmap
            float scale = lightSize.width / staticLightSize.width;
            auto lightSprite = m_shadowMapTexture->getSprite();
            lightSprite->setBlendFunc(cocos2d::BlendFunc::ADDITIVE);
            lightSprite->setPosition(lightRenderPosition);
            lightSprite->setTextureRect(cocos2d::Rect(cocos2d::Vec2::ZERO, lightScreenIntersect.size));
            lightSprite->setContentSize(lightScreenIntersect.size * scale);
            lightSprite->setGLProgram(cocos2d::GLProgramCache::getInstance()->getGLProgram(cocos2d::GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR));
            
            renderToTexture(m_shadowMapTexture->getSprite(), m_lightMapTexture);
        }
                
        if (m_model->getDrawDebug())
        {
            gameView->getDebugDrawNode()->drawRect(lightScreenRect.origin,
                                                   lightScreenRect.origin + lightScreenRect.size, cocos2d::Color4F::YELLOW);
            gameView->getDebugDrawNode()->drawRect(cocos2d::Vec2::ONE,
                                                   winSize - cocos2d::Size(1,1), cocos2d::Color4F::MAGENTA);

            gameView->getDebugDrawNode()->drawCircle(lightScreenPosition, lightRadius, 0.f, 16, false, light.color);
            gameView->getDebugDrawNode()->drawRect(lightScreenIntersect.origin, lightScreenIntersect.origin + lightScreenIntersect.size, cocos2d::Color4F::YELLOW);
        }
    }
        
    for (size_t deadLightID : deadLightIDs)
    {
        m_model->removeLight(deadLightID);
    }
}

void LightController::renderOccluders()
{
    const auto& gameView = Injector::globalInjector().getInstance<GameView>();
    const auto& occluders = gameView->getOccluderNode();
    occluders->setPosition(gameView->getView()->getPosition());
    
    m_occluderTexture->beginWithClear(0.f, 0.f, 0.f, 0.f);
    occluders->visit();
    m_occluderTexture->end();
    cocos2d::Director::getInstance()->getRenderer()->render();
    occluders->setPosition(cocos2d::Vec2::ZERO);
}

void LightController::renderOccludersToShadowMap(const cocos2d::Rect& lightScreenIntersect,
                                                 const cocos2d::Rect& lightScreenRect)
{
    bool clipTop = lightScreenRect.size.height > lightScreenIntersect.size.height && lightScreenRect.origin.y < lightScreenIntersect.origin.y;
    bool clipRight = lightScreenRect.size.width > lightScreenIntersect.size.width && lightScreenRect.origin.x < lightScreenIntersect.origin.x;
    cocos2d::Vec2 relativeOccluderPos = (lightScreenRect.size - lightScreenIntersect.size);
    relativeOccluderPos.x = clipRight ? relativeOccluderPos.x : 0.f;
    relativeOccluderPos.y = clipTop ? relativeOccluderPos.y : 0.f;
    
    // Resize occluder texture to draw only area intersected by light
    m_occluderTexture->getSprite()->setTextureRect(lightScreenIntersect);
    // Keep position such that reading from texture in shader is simplified
    m_occluderTexture->getSprite()->setPosition(relativeOccluderPos);
    
    // Temporarily use shadowmap texture to grab occluders
    m_occluderSliceTexture->beginWithClear(0.f, 0.f, 0.f, 0.f);
    m_occluderTexture->getSprite()->visit();
    m_occluderSliceTexture->end();
    cocos2d::Director::getInstance()->getRenderer()->render();

    // Debug shadowmap occluders
//    m_occluderSliceTexture->getSprite()->setTextureRect(cocos2d::Rect(cocos2d::Vec2::ZERO, lightScreenIntersect.size));
//    m_occluderSliceTexture->getSprite()->setPosition(cocos2d::Vec2::ZERO);
}

void LightController::renderDistanceToLight(const cocos2d::Rect& lightScreenRect,
                                            const cocos2d::Rect& lightScreenIntersect,
                                            const cocos2d::Vec2& lightScreenPosition,
                                            const float lightScreenRadius)
{
    // Resize shadowmap containing occluders
    const cocos2d::Rect occluderSliceTexRect = m_shadowMapTexture->getSprite()->getTextureRect();
    
    // Position occluders for reading to distorted to texture
    m_occluderSliceTexture->getSprite()->setTextureRect(cocos2d::Rect(cocos2d::Vec2::ZERO, lightScreenRect.size));
    m_occluderSliceTexture->getSprite()->setPosition(cocos2d::Vec2::ZERO);
    
    // Compute distance and distort rays into parallel lines
    cocos2d::GLProgramState* computeDistanceState = cocos2d::GLProgramState::getOrCreateWithGLProgram(m_computeDistanceShader);
    computeDistanceState->setUniformVec2("u_resolution", LIGHT_TEXTURE_SIZE);
    computeDistanceState->setUniformFloat("u_radius", lightScreenRadius);
    m_occluderSliceTexture->getSprite()->setGLProgram(m_computeDistanceShader);

    // Render distorted occluders with default distance of 1.0
    m_distortedTexture->beginWithClear(1.f, 1.f, 1.f, 0.f);
    m_occluderSliceTexture->getSprite()->visit();
    m_distortedTexture->end();
    cocos2d::Director::getInstance()->getRenderer()->render();

    // UNCOMMENT BELOW ONLY for debugging distorted texture
//    m_distortedTexture->getSprite()->setTextureRect(cocos2d::Rect(cocos2d::Vec2::ZERO, lightScreenRect.size));
//    m_distortedTexture->getSprite()->setPosition(cocos2d::Vec2(lightScreenRect.size.width * 2.f, 0.f));
}

void LightController::renderMinimumDistance(const cocos2d::Rect& lightScreenIntersect,
                                            const cocos2d::Vec2& lightIntersectPosition,
                                            const float lightScreenRadius)
{
    // Compute minimum light distance from distorted ray texture
    const float lightScreenSize = lightScreenRadius * 2.f;
    const cocos2d::Size minimumDistanceMapSize = cocos2d::Size(2, lightScreenSize);
    m_distortedTexture->getSprite()->setTextureRect(cocos2d::Rect(cocos2d::Vec2::ZERO, minimumDistanceMapSize));
    m_distortedTexture->getSprite()->setPosition(cocos2d::Vec2::ZERO);
    
    cocos2d::GLProgramState* minimumDistanceState = cocos2d::GLProgramState::getOrCreateWithGLProgram(m_minimumDistanceShader);
    minimumDistanceState->setUniformVec2("u_resolution", LIGHT_TEXTURE_SIZE);
    minimumDistanceState->setUniformFloat("u_radius", lightScreenRadius);
    m_distortedTexture->getSprite()->setGLProgram(m_minimumDistanceShader);
    
    m_minimumDistanceTexture->beginWithClear(0, 0, 0, 0);
    m_distortedTexture->getSprite()->visit();
    m_minimumDistanceTexture->end();
    cocos2d::Director::getInstance()->getRenderer()->render();

    // UNCOMMENT BELOW ONLY for debugging minimized texture
//    m_minimumDistanceTexture->getSprite()->setScaleX(10.f);
//    m_minimumDistanceTexture->getSprite()->setScaleY(3.f);
//    m_minimumDistanceTexture->getSprite()->setTextureRect(cocos2d::Rect(cocos2d::Vec2(0,0), minimumDistanceMapSize));
//    m_minimumDistanceTexture->getSprite()->setPositionY(lightScreenSize + 10.f);
}

void LightController::renderShadows(const cocos2d::Color4F& lightColor,
                                    const cocos2d::Vec2& lightScreenPosition,
                                    const float lightScreenRadius,
                                    const cocos2d::Rect& lightScreenRect,
                                    const cocos2d::Rect& lightScreenIntersect)
{
    bool clipTop = lightScreenRect.size.height > lightScreenIntersect.size.height && lightScreenRect.origin.y < lightScreenIntersect.origin.y;
    bool clipRight = lightScreenRect.size.width > lightScreenIntersect.size.width && lightScreenRect.origin.x < lightScreenIntersect.origin.x;
    cocos2d::Vec2 relativeOccluderPos = (lightScreenRect.size - lightScreenIntersect.size);
    relativeOccluderPos.x = clipRight ? relativeOccluderPos.x : 0.f;
    relativeOccluderPos.y = clipTop ? relativeOccluderPos.y : 0.f;

    // Resize shadowmap to draw only light area
    m_occluderSliceTexture->getSprite()->setTextureRect(cocos2d::Rect(relativeOccluderPos, lightScreenIntersect.size));

    cocos2d::GLProgramState* shadowState = cocos2d::GLProgramState::getOrCreateWithGLProgram(m_shadowMapNoBlurShader);
    shadowState->setUniformTexture("u_shadowTexture", m_minimumDistanceTexture->getSprite()->getTexture());
    shadowState->setUniformVec4("u_color", cocos2d::Vec4(lightColor.r, lightColor.g, lightColor.b, lightColor.a));
    shadowState->setUniformVec2("u_resolution", LIGHT_TEXTURE_SIZE);
    shadowState->setUniformFloat("u_radius", lightScreenRadius);
    m_occluderSliceTexture->getSprite()->setGLProgram(m_shadowMapNoBlurShader);

    // Render shadow texture
    m_shadowMapTexture->beginWithClear(0, 0, 0, 0);
    m_occluderSliceTexture->getSprite()->visit();
    m_shadowMapTexture->end();
    cocos2d::Director::getInstance()->getRenderer()->render();
        
    // UNCOMMENT BELOW ONLY for debugging shadowmap texture
//    auto standardShader = cocos2d::GLProgramCache::getInstance()->getGLProgram(cocos2d::GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR);
//    m_occluderSliceTexture->getSprite()->setGLProgram(standardShader);
//    m_distortedTexture->getSprite()->setGLProgram(standardShader);
//    m_shadowMapTexture->getSprite()->setGLProgram(standardShader);
//
//    m_occluderSliceTexture->getSprite()->setTextureRect(cocos2d::Rect(cocos2d::Vec2::ZERO, lightScreenRect.size));
//    m_occluderSliceTexture->getSprite()->setPosition(cocos2d::Vec2::ZERO);
//
//    m_distortedTexture->getSprite()->setTextureRect(cocos2d::Rect(cocos2d::Vec2::ZERO, lightScreenRect.size));
//    m_distortedTexture->getSprite()->setPosition(cocos2d::Vec2(lightScreenRect.size.width * 0.5f, 0.f));
//
//    m_shadowMapTexture->getSprite()->setTextureRect(cocos2d::Rect(cocos2d::Vec2::ZERO, lightScreenRect.size));
//    m_shadowMapTexture->getSprite()->setPosition(cocos2d::Vec2(lightScreenRect.size.width, 0.f));
//    m_shadowMapTexture->getSprite()->setBlendFunc({GL_SRC_ALPHA, GL_SRC_COLOR});
}

void LightController::renderLight(const cocos2d::Color4F& lightColor,
                                  const float lightScreenRadius)
{
    const cocos2d::Size lightScreenSize = cocos2d::Size(lightScreenRadius * 2.f, lightScreenRadius * 2.f);
    
    // Resize sprite to draw only light area
    m_distortedTexture->getSprite()->setTextureRect(cocos2d::Rect(cocos2d::Vec2(0, 0), lightScreenSize));
    m_distortedTexture->getSprite()->setContentSize(lightScreenSize);
    
    cocos2d::GLProgramState* lightState = cocos2d::GLProgramState::getOrCreateWithGLProgram(m_lightShader);
    lightState->setUniformVec4("u_color", cocos2d::Vec4(lightColor.r, lightColor.g, lightColor.b, lightColor.a));
    lightState->setUniformVec2("u_resolution", LIGHT_TEXTURE_SIZE);
    lightState->setUniformFloat("u_radius", lightScreenRadius);
    m_distortedTexture->getSprite()->setGLProgram(m_lightShader);
    
    // Render light to texture
    m_distortedTexture->getSprite()->setPosition(cocos2d::Vec2::ZERO);
    m_shadowMapTexture->beginWithClear(0, 0, 0, 0);
    m_distortedTexture->getSprite()->visit();
    m_shadowMapTexture->end();
    cocos2d::Director::getInstance()->getRenderer()->render();
}

void LightController::renderToTexture(cocos2d::Sprite* sprite,
                                      cocos2d::RenderTexture* texture)
{
    texture->begin();
    sprite->visit();
    texture->end();
    cocos2d::Director::getInstance()->getRenderer()->render();
}

void LightController::onAddLight(const Event& event)
{
    const AddLightEvent& addLightEvent = static_cast<const AddLightEvent&>(event);
    m_model->addLight(addLightEvent.getLight());
}

void LightController::onRemoveLight(const Event& event)
{
    const RemoveLightEvent& removeLightEvent = static_cast<const RemoveLightEvent&>(event);
    m_model->removeLight(removeLightEvent.getLightID());
}
