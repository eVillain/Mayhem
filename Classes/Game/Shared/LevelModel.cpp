#include "LevelModel.h"
#include "Core/Dispatcher.h"
#include "CollisionUtils.h"
#include "SharedConstants.h"
#include "AddLightEvent.h"

LevelModel::LevelModel()
: m_tileMap(nullptr)
{
}

void LevelModel::loadLevel(const std::string& level,
                               const bool lights)
{
    const std::string levelPath = "res/tilemaps/" + level;
    loadTileMap(levelPath);
    
    if (m_tileMap && lights)
    {
        loadLights();
    }
    
    m_level = m_tileMap ? level : "";
}

void LevelModel::unloadLevel()
{
    m_level = "";
    m_tileMap = nullptr;
}

const cocos2d::Size LevelModel::getMapSize() const
{
    if (!m_tileMap)
    {
        return cocos2d::Size();
    }
    const cocos2d::Size& tileSize = m_tileMap->getTileSize();
    const cocos2d::Size& mapSize = m_tileMap->getMapSize();
    return cocos2d::Size(tileSize.width * mapSize.width,
                         tileSize.height * mapSize.height);
}

void LevelModel::loadTileMap(const std::string& tileMap)
{
    // create a TMX map
    m_tileMap = cocos2d::TMXTiledMap::create(tileMap);
    
    loadTileWalls();
}

void LevelModel::loadTileWalls()
{
    // Create physics for tiles
    cocos2d::TMXLayer* meta = m_tileMap->getLayer("Meta");
    
    if (!meta)
    {
        return;
    }

    const cocos2d::Size& tileSize = m_tileMap->getTileSize();
    const float tileWidth = tileSize.width;
    const float tileHeight = tileSize.height;
    const float mapHeight = tileHeight * m_tileMap->getMapSize().height;
    int totalStaticBlocks = 0;
    
    for (int column = 0; column < m_tileMap->getMapSize().height; column++)
    {
        bool previousTileCollidable = false;
        cocos2d::Rect box;
        
        for (int row = 0; row < m_tileMap->getMapSize().width; row++)
        {
            const uint32_t tileGID = meta->getTileGIDAt(cocos2d::Vec2(row, column));
            cocos2d::Value tileProperties = m_tileMap->getPropertiesForGID(tileGID);
            if (tileProperties.getType() != cocos2d::Value::Type::MAP)
            {
                if (previousTileCollidable)
                {
                    m_staticRects.push_back(box);
                    ++totalStaticBlocks;
                    previousTileCollidable = false;
                }
                continue;
            }
            
            bool foundCollidable = false;
            cocos2d::ValueMap& values = tileProperties.asValueMap();
            for(auto& keyval : values)
            {
                const std::string key = keyval.first;
                if (key == "Collidable")
                {
                    foundCollidable = true;
                }
//                else
//                {
//                    if (key == "Breakable")
//                    {
//                        box = cocos2d::Rect(tileWidth*row, mapHeight-(tileHeight*col), tileWidth, tileHeight);
//                        foundCollidable = true;
//                    }
//                }
            }
            if (foundCollidable)
            {
                if (previousTileCollidable)
                {
                    box.size.width = (tileWidth * (row + 1)) - box.origin.x;
                }
                else
                {
                    box = cocos2d::Rect(tileWidth * row, mapHeight - (tileHeight * (column + 1)), tileWidth, tileHeight);
                    previousTileCollidable = true;
                 }
            }
            else if (previousTileCollidable)
            {
                m_staticRects.push_back(box);
                ++totalStaticBlocks;
                previousTileCollidable = false;
            }
        }
        if (previousTileCollidable)
        {
            m_staticRects.push_back(box);
            ++totalStaticBlocks;
            previousTileCollidable = false;
        }
    }
//    mergeTileWalls();
}

void LevelModel::mergeTileWalls()
{
//	std::vector<cocos2d::Rect> mergedRects;
//
//    for (auto it = m_staticRects.begin(); it != m_staticRects.end(); it++)
//    {
//        auto& rect = *it;
//        for (auto it2 = m_staticRects.begin(); it2 != m_staticRects.end(); it2++)
//        {
//            if (it == it2)
//            {
//                continue;
//            }
//            const auto& rect2 = *it2;
//            if (rect.origin.x == rect2.origin.x &&
//                rect.size.width == rect2.size.width &&
//                rect.origin.y == rect2.origin.y + rect2.size.height)
//            {
//                // Merge rects
//                rect.origin.y = rect2.origin.y;
//                rect.size.height += rect2.size.height;
////				mergedRects.push_back(rect);
//            }
//        }
//    }
//	m_staticRects = mergedRects;
}

void LevelModel::loadLights()
{
    cocos2d::TMXObjectGroup* objectGroup = m_tileMap->getObjectGroup("Lights");
    if (!objectGroup)
    {
        return;
    }
    const auto& objects = objectGroup->getObjects();
    for (const auto& object : objects)
    {
        if (object.getType() != cocos2d::Value::Type::MAP)
        {
            continue;
        }

        cocos2d::Vec2 lightPosition;
        float lightRadius = 1.f;
        cocos2d::Color4F lightColor = cocos2d::Color4F::BLACK;
        
        const auto& values = object.asValueMap();
        for (const auto& value : values)
        {
            if (value.first == "Red")
            {
                lightColor.r = std::stof(value.second.asString()) / 255.f;
            }
            else if (value.first == "Green")
            {
                lightColor.g = std::stof(value.second.asString()) / 255.f;
            }
            else if (value.first == "Blue")
            {
                lightColor.b = std::stof(value.second.asString()) / 255.f;
            }
            else if (value.first == "width" ||
                     value.first == "height")
            {
                float radius = value.second.asFloat() * 0.5f;
                if (radius > lightRadius)
                {
                    lightRadius = radius;
                }
            }
            else if (value.first == "x")
            {
                lightPosition.x = value.second.asFloat();
            }
            else if (value.first == "y")
            {
                lightPosition.y = value.second.asFloat();
            }
        }
        
        Dispatcher::globalDispatcher().dispatch(AddLightEvent({lightPosition + cocos2d::Vec2(lightRadius,lightRadius), lightRadius, lightColor, -1.f}));
    }
}

const cocos2d::Vec2 LevelModel::getRandomTile() const
{
    const cocos2d::Size mapSize = m_tileMap->getMapSize();
    const cocos2d::Size tileSize = m_tileMap->getTileSize();
    float randX = cocos2d::RandomHelper::random_real(tileSize.width, (mapSize.width-1) * tileSize.width);
    float randY = cocos2d::RandomHelper::random_real(tileSize.height, (mapSize.height-1) * tileSize.height);
    cocos2d::Vec2 randomTile = cocos2d::Vec2(randX / tileSize.width, mapSize.height - (randY / tileSize.height));
    while (true)
    {
        if (!isTileSolid(randomTile))
        {
            break;
        }
        randX = cocos2d::RandomHelper::random_real(tileSize.width, (mapSize.width-1) * tileSize.width);
        randY = cocos2d::RandomHelper::random_real(tileSize.height, (mapSize.height-1) * tileSize.height);
        randomTile = cocos2d::Vec2(randX / tileSize.width, mapSize.height - (randY / tileSize.height));
    }
    
    return randomTile;
}

bool LevelModel::isTileSolid(const cocos2d::Vec2 &position) const
{
    cocos2d::TMXLayer* meta = m_tileMap->getLayer("Meta");
    if (!meta)
    {
        return false;
    }

    const uint32_t tileGID = meta->getTileGIDAt(position);
    const cocos2d::Value& tileProperties = m_tileMap->getPropertiesForGID(tileGID);
    if (tileProperties.getType() != cocos2d::Value::Type::MAP)
    {
        return false;
    }
    
    const cocos2d::ValueMap& values = tileProperties.asValueMap();
    for(const auto& keyval : values)
    {
        const std::string& key = keyval.first;
        if (key == "Collidable")
        {
            return true;
        }
    }
    return false;
}
