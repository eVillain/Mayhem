#ifndef LevelView_h
#define LevelView_h

#include "cocos2d.h"
#include "cocos-ext.h"

class LevelView : public cocos2d::Node
{
public:
    LevelView();
    
    CREATE_FUNC(LevelView);

    void setTileMap(const cocos2d::RefPtr<cocos2d::TMXTiledMap>& tileMap);
    const cocos2d::RefPtr<cocos2d::TMXTiledMap>& getTileMap() { return m_tileMap; }
    
    const cocos2d::RefPtr<cocos2d::TMXLayer>& getBGTiles() const { return m_bgTilesNode; }
    const cocos2d::RefPtr<cocos2d::TMXLayer>& getFGTiles() const { return m_fgTilesNode; }
    const cocos2d::RefPtr<cocos2d::TMXLayer>& getMetaTiles() const { return m_metaTilesNode; }
    const cocos2d::RefPtr<cocos2d::TMXLayer>& getStaticLightingTiles() const { return m_staticLightingNode; }
    const cocos2d::RefPtr<cocos2d::TMXLayer>& getSelfLightingTiles() const { return m_selfLightingNode; }

private:
    cocos2d::RefPtr<cocos2d::TMXTiledMap> m_tileMap;
    cocos2d::RefPtr<cocos2d::TMXLayer> m_bgTilesNode;
    cocos2d::RefPtr<cocos2d::TMXLayer> m_fgTilesNode;
    cocos2d::RefPtr<cocos2d::TMXLayer> m_metaTilesNode;
    cocos2d::RefPtr<cocos2d::TMXLayer> m_staticLightingNode;
    cocos2d::RefPtr<cocos2d::TMXLayer> m_selfLightingNode;
};

#endif /* LevelView_h */
