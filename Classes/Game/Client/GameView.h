#ifndef GameView_h
#define GameView_h

#include "cocos2d.h"
#include "cocos-ext.h"

class GameSettings;
class Pseudo3DItem;

class GameView
{
public:
    static const std::string SETTING_RENDER_DEFERRED;
    static const std::string SETTING_RENDER_POSTPROCESS;
    static const std::string SETTING_RENDER_LINE_OF_SIGHT;

    GameView(std::shared_ptr<GameSettings> gameSettings);
    ~GameView();
    
    void initialize();
    
    cocos2d::RefPtr<cocos2d::RenderTexture> getRenderTexture() const { return m_renderTexture; }
    cocos2d::RefPtr<cocos2d::Node> getView() const { return m_view; }
    cocos2d::RefPtr<cocos2d::Node> getGameRootNode() const { return m_gameRootNode; }
    cocos2d::RefPtr<cocos2d::Node> getBackgroundNode() const { return m_backgroundNode; }
    cocos2d::RefPtr<cocos2d::Node> getOccluderNode() const { return m_occluderNode; }
    cocos2d::RefPtr<cocos2d::TMXLayer> getBGTilesNode() const { return m_bgTilesNode; }
    cocos2d::RefPtr<cocos2d::TMXLayer> getFGTilesNode() const { return m_fgTilesNode; }
    cocos2d::RefPtr<cocos2d::TMXLayer> getStaticLightingNode() const { return m_staticLightingNode; }
    cocos2d::RefPtr<cocos2d::Node> getSelfLightingNode() const { return m_selfLightingNode; }

    cocos2d::RefPtr<cocos2d::DrawNode> getPixelDrawNode() { return m_pixelDrawNode; }
    cocos2d::RefPtr<cocos2d::DrawNode> getDebugDrawNode() { return m_debugDrawNode; }

    void setTileMap(cocos2d::RefPtr<cocos2d::TMXTiledMap> tileMap);
    cocos2d::RefPtr<cocos2d::TMXTiledMap> getTileMap() { return m_tileMap; }
    
    void setSpriteBatch(cocos2d::RefPtr<cocos2d::SpriteBatchNode> batch);
    cocos2d::RefPtr<cocos2d::SpriteBatchNode> getSpriteBatch() { return m_spriteBatch; }

    cocos2d::RefPtr<cocos2d::Node> getDebugLabelNode() const { return m_debugLabelNode; }
    void addDebugLabel(const std::string& labelID, const std::string& text);
        
    void createPseudo3DParticle(const cocos2d::Color4F& color,
                                const cocos2d::Vec2& position,
                                const float positionZ,
                                const cocos2d::Vec2& velocity,
                                const float velocityZ,
                                const float lifeTime,
                                const float elasticity);
    
    void createPseudo3DSprite(const std::string& spriteName,
                              const cocos2d::Vec2& position,
                              const float positionZ,
                              const cocos2d::Vec2& velocity,
                              const float velocityZ,
                              const float lifeTime,
                              const float elasticity);
    
    std::vector<std::shared_ptr<Pseudo3DItem>>& getPseudo3DItems() { return m_pseudo3DItems; }

    void renderToTexture();
    
    cocos2d::RefPtr<cocos2d::Animation> createAnimation(const std::string frameName,
                                                        const size_t numFrames,
                                                        const float frameDelay);
    
    void drawExplosion(const cocos2d::Vec2& position);
    void updateLineOfSight(const cocos2d::Vec2 playerPosition,
                           const std::vector<cocos2d::Rect>& staticRects);
        
    const cocos2d::Vec2 toViewPosition(const cocos2d::Vec2& point) const;
    
    static constexpr int Z_ORDER_PLAYER_BEHIND = 4;
    static constexpr int Z_ORDER_PLAYER = 5;
    static constexpr int Z_ORDER_PLAYER_ITEM = 6;
    static constexpr int Z_ORDER_PLAYER_FRONT = 7;
    static constexpr int Z_ORDER_ITEM = 10;
    static constexpr int Z_ORDER_PROJECTILE_TRAIL = 11;
    static constexpr int Z_ORDER_PROJECTILE = 12;
    static constexpr int Z_ORDER_GUN_MUZZLE = 20;
    static constexpr int Z_ORDER_TEXT_BLIMP = 30;
    static constexpr int Z_ORDER_BACKGROUND = -1;

private:
    std::shared_ptr<GameSettings> m_gameSettings;
    
    cocos2d::RefPtr<cocos2d::RenderTexture> m_renderTexture;
    cocos2d::RefPtr<cocos2d::Node> m_view;
    cocos2d::RefPtr<cocos2d::Node> m_gameRootNode;
    cocos2d::RefPtr<cocos2d::Node> m_backgroundNode;
    cocos2d::RefPtr<cocos2d::Node> m_occluderNode;
    cocos2d::RefPtr<cocos2d::Node> m_selfLightingNode;

    cocos2d::RefPtr<cocos2d::TMXTiledMap> m_tileMap;
    cocos2d::RefPtr<cocos2d::TMXLayer> m_bgTilesNode;
    cocos2d::RefPtr<cocos2d::TMXLayer> m_fgTilesNode;
    cocos2d::RefPtr<cocos2d::TMXLayer> m_metaTilesNode;
    cocos2d::RefPtr<cocos2d::TMXLayer> m_staticLightingNode;

    cocos2d::RefPtr<cocos2d::SpriteBatchNode> m_spriteBatch;

    cocos2d::RefPtr<cocos2d::DrawNode> m_pixelDrawNode;
    cocos2d::RefPtr<cocos2d::DrawNode> m_debugDrawNode;
    cocos2d::RefPtr<cocos2d::DrawNode> m_lineOfSightNode;
    
    cocos2d::RefPtr<cocos2d::Node> m_debugLabelNode;
    std::map<std::string, cocos2d::Label*> m_labels;

    std::vector<std::shared_ptr<Pseudo3DItem>> m_pseudo3DItems;
    
    static constexpr int Z_ORDER_PHYSICS_DEBUG = 100;
    static constexpr int Z_ORDER_GAME_SPRITES = 5;
    static constexpr int Z_ORDER_GAME_TILEMAP = 3;

    static const std::string SPRITE_BATCH_FILE;
    static const std::string SPRITE_BATCH_TEXTURE_FILE;
};

#endif /* GameView_h */
