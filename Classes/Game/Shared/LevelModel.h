#ifndef LevelModel_h
#define LevelModel_h

#include "cocos2d.h"

class LevelModel
{
public:
    LevelModel();
    
    cocos2d::TMXTiledMap* getTileMap() const { return m_tileMap; }
    const std::vector<cocos2d::Rect>& getStaticRects() const { return m_staticRects; }

    void loadLevel(const std::string& level,
                   const bool lights = true);
    const cocos2d::Size getMapSize() const;

    const cocos2d::Vec2 getRandomTile() const;
    bool isTileSolid(const cocos2d::Vec2& tilePos) const;

    const std::string& getLevel() const { return m_level; }
private:
    cocos2d::RefPtr<cocos2d::TMXTiledMap> m_tileMap;
    std::vector<cocos2d::Rect> m_staticRects;
    std::string m_level;
    
    void loadTileMap(const std::string& tileMap);
    void loadTileWalls();
    void mergeTileWalls();
    void loadLights();
};

#endif /* LevelModel_h */
