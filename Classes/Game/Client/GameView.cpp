#include "GameView.h"

#include "Pseudo3DParticle.h"
#include "Pseudo3DSprite.h"
#include "HUDHelper.h"
#include "GameSettings.h"
#include "GameViewConstants.h"

GameView::GameView(std::shared_ptr<GameSettings> gameSettings)
: m_gameSettings(gameSettings)
, m_renderTexture(nullptr)
, m_view(cocos2d::Node::create())
, m_gameRootNode(cocos2d::Node::create())
, m_backgroundNode(cocos2d::Node::create())
, m_occluderNode(cocos2d::Node::create())
, m_selfLightingNode(cocos2d::Node::create())
, m_tileMap(nullptr)
, m_bgTilesNode(nullptr)
, m_fgTilesNode(nullptr)
, m_metaTilesNode(nullptr)
, m_staticLightingNode(nullptr)
, m_spriteBatch(nullptr)
, m_debugDrawNode(cocos2d::DrawNode::create())
, m_pixelDrawNode(cocos2d::DrawNode::create())
, m_lineOfSightNode(cocos2d::DrawNode::create())
, m_debugLabelNode(cocos2d::Node::create())
{
    printf("GameView:: constructor: %p\n", this);
}

GameView::~GameView()
{
    printf("GameView:: destructor: %p\n", this);
}

void GameView::initialize()
{
    m_debugDrawNode->setVisible(false);
    for (const auto& pair : m_labels)
    {
        pair.second->setVisible(false);
    }
    
    cocos2d::SpriteFrameCache* cache = cocos2d::SpriteFrameCache::getInstance();
    cache->addSpriteFramesWithFile(GameViewConstants::SPRITE_BATCH_FILE);
    
    auto batch = cocos2d::SpriteBatchNode::create(GameViewConstants::SPRITE_BATCH_TEXTURE_FILE);
    batch->getTexture()->setAliasTexParameters();
    setSpriteBatch(batch);
    
    auto director = cocos2d::Director::getInstance();
    cocos2d::Size designSize = director->getWinSize();
    //    cocos2d::Size renderSize = cocos2d::Director::getInstance()->getWinSizeInPixels();
    // TODO: Use actual render size here
    const cocos2d::Value& deferredRenderSetting = m_gameSettings->getValue(GameViewConstants::SETTING_RENDER_DEFERRED, cocos2d::Value(true));
    if (deferredRenderSetting.asBool())
    {
        m_renderTexture = cocos2d::RenderTexture::create(designSize.width, designSize.height,
                                                         cocos2d::Texture2D::PixelFormat::RGBA8888,
                                                         GL_DEPTH24_STENCIL8);
    }
    
    m_gameRootNode->addChild(m_backgroundNode, 0);
    m_gameRootNode->addChild(m_occluderNode, 2);
    m_gameRootNode->addChild(m_pixelDrawNode, 10);
    m_gameRootNode->addChild(m_lineOfSightNode, 100);

    m_view->addChild(m_gameRootNode, 0);
}

void GameView::shutdown()
{
    m_renderTexture = nullptr;
    m_tileMap = nullptr;
    m_bgTilesNode = nullptr;
    m_fgTilesNode = nullptr;
    m_metaTilesNode = nullptr;
    m_staticLightingNode = nullptr;
    m_spriteBatch = nullptr;
    m_tileMap = nullptr;
    
    m_occluderNode->removeAllChildren();
    m_backgroundNode->removeAllChildren();
    m_gameRootNode->removeAllChildren();
    m_view->removeChild(m_gameRootNode);
    
    m_pseudo3DItems.clear();
}

void GameView::setTileMap(cocos2d::RefPtr<cocos2d::TMXTiledMap> tileMap)
{
    if (m_tileMap)
    {
        m_backgroundNode->removeChild(m_tileMap);
    }
    m_tileMap = tileMap;
    m_backgroundNode->addChild(m_tileMap);
    m_bgTilesNode = m_tileMap->getLayer("Background");
    m_fgTilesNode = m_tileMap->getLayer("Foreground");
    if (m_fgTilesNode)
    {
        m_fgTilesNode->removeFromParent();
        m_gameRootNode->addChild(m_fgTilesNode, 1);
    }
    
    m_staticLightingNode = m_tileMap->getLayer("StaticLights");
    auto selfLitTiles = m_tileMap->getLayer("SelfLit");
    selfLitTiles->removeFromParent();
    m_selfLightingNode->addChild(selfLitTiles);
    
    m_metaTilesNode = m_tileMap->getLayer("Meta");
    m_metaTilesNode->setVisible(false);
}

void GameView::setSpriteBatch(cocos2d::RefPtr<cocos2d::SpriteBatchNode> batch)
{
    if (m_spriteBatch)
    {
        m_occluderNode->removeChild(m_spriteBatch, true);
    }
    
    m_spriteBatch = batch;
    
    if (m_spriteBatch)
    {
        m_occluderNode->addChild(m_spriteBatch);
    }
}

void GameView::addDebugLabel(const std::string& labelID, const std::string& text)
{
    if (!m_labels.count(labelID))
    {
        auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

        auto label = HUDHelper::createLabel3x6(text, 24.f);
        label->setAlignment(cocos2d::TextHAlignment::LEFT);
        label->setAnchorPoint(cocos2d::Vec2(0.f, 1.0f));
        label->setPosition(cocos2d::Vec2(10.f, visibleSize.height - (m_labels.size() * 10.f)));
        m_debugLabelNode->addChild(label);
        m_labels[labelID] = label;
    }
    
    m_labels.at(labelID)->setString(text);
    m_labels.at(labelID)->setVisible(m_debugDrawNode->isVisible());
}

void GameView::createPseudo3DParticle(const cocos2d::Color4F& color,
                                      const cocos2d::Vec2& position,
                                      const float positionZ,
                                      const cocos2d::Vec2& velocity,
                                      const float velocityZ,
                                      const float lifeTime,
                                      const float elasticity)
{
    auto particle = std::make_shared<Pseudo3DParticle>(color, position, positionZ, velocity, velocityZ, lifeTime, elasticity);
    m_pseudo3DItems.push_back(particle);
}

void GameView::createPseudo3DSprite(const std::string& spriteName,
                                    const cocos2d::Vec2& position,
                                    const float positionZ,
                                    const cocos2d::Vec2& velocity,
                                    const float velocityZ,
                                    const float lifeTime,
                                    const float elasticity)
{
    cocos2d::Sprite* sprite = cocos2d::Sprite::createWithSpriteFrameName(spriteName);
    m_gameRootNode->addChild(sprite, GameViewConstants::Z_ORDER_GAME_SPRITES);
    auto pseudoSprite = std::make_shared<Pseudo3DSprite>(sprite, position, positionZ, velocity, velocityZ, lifeTime, elasticity);
    m_pseudo3DItems.push_back(pseudoSprite);
}

void GameView::createRenderTexture()
{
    auto director = cocos2d::Director::getInstance();
    cocos2d::Size designSize = director->getWinSize();
    m_renderTexture = cocos2d::RenderTexture::create(designSize.width, designSize.height,
                                                     cocos2d::Texture2D::PixelFormat::RGBA8888,
                                                     GL_DEPTH24_STENCIL8);
}

void GameView::renderToTexture()
{
    m_pixelDrawNode->setLineWidth(1.f);

    m_gameRootNode->setVisible(true);
    m_gameRootNode->setPosition(m_view->getPosition());
    m_renderTexture->beginWithClear(0.f, 0.f, 0.f, 0.f);
    m_gameRootNode->visit();
    m_renderTexture->end();
    cocos2d::Director::getInstance()->getRenderer()->render();

    m_gameRootNode->setVisible(false);
    m_gameRootNode->setPosition(cocos2d::Vec2::ZERO);
}

cocos2d::RefPtr<cocos2d::Animation> GameView::createAnimation(const std::string frameName,
                                                                const size_t numFrames,
                                                                const float frameDelay)
{
    cocos2d::SpriteFrameCache* cache = cocos2d::SpriteFrameCache::getInstance();
    
    cocos2d::Vector<cocos2d::SpriteFrame*> animFrames((int)numFrames);
    
    char str[100] = {0};
    for (size_t i = 1; i <= numFrames; i++)
    {
        sprintf(str, "%s%02zu.png", frameName.c_str(), i);
        cocos2d::SpriteFrame* frame = cache->getSpriteFrameByName( str );
        animFrames.pushBack(frame);
    }
    
    cocos2d::RefPtr<cocos2d::Animation> animation = cocos2d::Animation::createWithSpriteFrames(animFrames, frameDelay);
    return animation;
}

void GameView::drawExplosion(const cocos2d::Vec2& position)
{
    auto sprite = cocos2d::Sprite::createWithSpriteFrameName("Explosion1-01.png");
    sprite->setPosition(position);
    cocos2d::RefPtr<cocos2d::Animation> explosionAnim = createAnimation("Explosion1-", 10, 0.06f);
    m_backgroundNode->addChild(sprite, GameViewConstants::Z_ORDER_GAME_SPRITES);
    cocos2d::CallFunc* removeFromParent = cocos2d::CallFunc::create([sprite](){
        sprite->removeFromParent();
    });
    cocos2d::Sequence* sequence = cocos2d::Sequence::create(cocos2d::Animate::create(explosionAnim),
                                                            removeFromParent,
                                                            NULL);
    sprite->runAction(sequence);
}

void GameView::updateLineOfSight(const cocos2d::Vec2 playerPosition,
                                 const std::vector<cocos2d::Rect>& staticRects)
{
    m_lineOfSightNode->clear();
    
    for (const cocos2d::Rect& rect : staticRects)
    {
        const std::vector<cocos2d::Vec2> rectCorners =
        {
            rect.origin, // bl
            rect.origin + cocos2d::Vec2(0.f, rect.size.height), // tl
            rect.origin + cocos2d::Vec2(rect.size.width, rect.size.height), // tr
            rect.origin + cocos2d::Vec2(rect.size.width, 0.f) // br
        };

        for (size_t i = 0; i < 4; i++)
        {
            const size_t i2 = i == 3 ? 0 : i + 1;
            const cocos2d::Vec2& p1 = rectCorners.at(i);
            const cocos2d::Vec2& p2 = rectCorners.at(i2);
            const cocos2d::Vec2 midPoint = p1 + ((p2 - p2) * 0.5f);
            const cocos2d::Vec2 playerToLineMidPoint = midPoint - playerPosition;
            const cocos2d::Vec2 normal = (p2-p1).getPerp().getNormalized();
            float dotProduct = normal.dot(playerToLineMidPoint.getNormalized());
            
            if (dotProduct > 0.f)
            {
                const float PROJECT_DISTANCE = 500.f;
                const cocos2d::Vec2 projectedP1 = p1 + (p1 - playerPosition).getNormalized() * PROJECT_DISTANCE;
                const cocos2d::Vec2 projectedP2 = p2 + (p2 - playerPosition).getNormalized() * PROJECT_DISTANCE;
                const cocos2d::Vec2 verts[6] = {
                    p1, p2, projectedP1,
                    projectedP1, projectedP2, p2,
                };
                
                m_lineOfSightNode->drawSolidPoly(verts, 6, cocos2d::Color4F::BLACK);
            }
        }
    }
}

const cocos2d::Vec2 GameView::toViewPosition(const cocos2d::Vec2& point) const
{
    const cocos2d::Value& deferredRenderSetting = m_gameSettings->getValue(GameViewConstants::SETTING_RENDER_DEFERRED, cocos2d::Value(true));
    const float viewScale = deferredRenderSetting.asBool() ? m_renderTexture->getSprite()->getScale() : m_view->getScale();
    auto director = cocos2d::Director::getInstance();
    const cocos2d::Vec2 midWindow = director->getWinSize() * 0.5f;
    const cocos2d::Vec2 pos = (point + m_view->getPosition()) - midWindow;
    return (pos * viewScale) + (midWindow);
}

void GameView::toggleDebugDraw()
{
    if (m_debugDrawNode)
    {
        m_debugDrawNode->setVisible(!m_debugDrawNode->isVisible());
        for (const auto& pair : m_labels)
        {
            pair.second->setVisible(m_debugDrawNode->isVisible());
        }
    }
}
