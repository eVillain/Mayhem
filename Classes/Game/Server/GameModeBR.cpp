#include "GameModeBR.h"
#include "Game/Shared/EntityConstants.h"

#include "LevelModel.h"
#include "EntitiesModel.h"
#include "EntitiesController.h"

GameModeBR::GameModeBR(std::shared_ptr<EntitiesController> entitiesController,
           std::shared_ptr<EntitiesModel> entitiesModel,
           std::shared_ptr<LevelModel> levelModel,
           const int seed)
: GameMode(entitiesController, entitiesModel, levelModel)
, m_seed(seed)
, m_tileUpdateAccumulator(0.f)
, m_tileUpdateLayer(0)
{
}

void GameModeBR::update(const float deltaTime)
{
    static const float DEAD_TILE_REMOVE_PERIOD = 0.2f;
    m_tileUpdateAccumulator += deltaTime;
    
    while (m_tileUpdateAccumulator > DEAD_TILE_REMOVE_PERIOD)
    {
        m_tileUpdateAccumulator -= DEAD_TILE_REMOVE_PERIOD;
        
        removeNextDeadTile();
    }
}

void GameModeBR::onLevelLoaded(const bool isHost)
{
    if (isHost)
    {
        spawnThings();
        
        fillDeadTiles();
    }
}

void GameModeBR::onPlayerReady(const uint8_t playerID)
{
    
}

void GameModeBR::onPlayerDied(const uint8_t playerID)
{
    
}

void GameModeBR::spawnThings()
{
    for (int i = EntityType::Item_First_Placeholder + 1; i < EntityType::Item_Railgun + 1; i++)
    {
        m_entitiesController->createItem(m_entitiesModel->getNextEntityID(),
                                         (EntityType)i,
                                         cocos2d::Vec2(64 + 30 * i, 80),
                                         0.f);
        m_entitiesModel->incrementNextEntityID();
    }
    for (int i = EntityType::Item_Nade_Frag; i < EntityType::Item_Nade_Smoke + 1; i++)
    {
        m_entitiesController->createItem(m_entitiesModel->getNextEntityID(),
                                         (EntityType)i,
                                         cocos2d::Vec2(10 * i, 100),
                                         0.f);
        m_entitiesModel->incrementNextEntityID();
    }
    for (int i = EntityType::Item_Ammo_9mm; i < EntityType::Item_Last_Placeholder; i++)
    {
        m_entitiesController->createItem(m_entitiesModel->getNextEntityID(),
                                         (EntityType)i,
                                         cocos2d::Vec2(10 * i - 700, 120),
                                         0.f);
        m_entitiesModel->incrementNextEntityID();
    }
    for (int i = EntityType::Item_Helmet_Level2; i <= EntityType::Item_Helmet_Level3; i++)
    {
        m_entitiesController->createItem(m_entitiesModel->getNextEntityID(),
                                         (EntityType)i,
                                         cocos2d::Vec2(10 * i - 700, 120),
                                         0.f);
        m_entitiesModel->incrementNextEntityID();
    }
}

void GameModeBR::fillDeadTiles()
{
    const cocos2d::Size mapSize = m_levelModel->getTileMap()->getMapSize();
    const cocos2d::Vec2 centerTile = cocos2d::Vec2((mapSize.width / 2) - 1, (mapSize.height / 2) - 1);
    const int numRings = std::max((mapSize.width / 2), (mapSize.height / 2));
    
    for (int ring = 1; ring < numRings; ring++)
    {
        const cocos2d::Vec2 bottomLeft = cocos2d::Vec2(std::max(centerTile.x - (ring), 0.f),
                                                       std::max(centerTile.y - (ring), 0.f));
        const int ringWidth = (ring + 1) * 2;
        for (int x = 0; x < ringWidth; x++)
        {
            if (bottomLeft.x + x >= mapSize.width)
            {
                continue;
            }
            else if (bottomLeft.x + x < 0)
            {
                continue;
            }
            const cocos2d::Vec2 posTop = bottomLeft + cocos2d::Vec2(x, ringWidth - 1);
            const cocos2d::Vec2 posBottom = bottomLeft + cocos2d::Vec2(x, 0);
            m_deadTiles.push_back(posTop);
            m_deadTiles.push_back(posBottom);
        }
        for (int y = 0; y < ringWidth; y++)
        {
            if (bottomLeft.y + y >= mapSize.height)
            {
                continue;
            }
            else if (bottomLeft.y + y < 0)
            {
                continue;
            }
            const cocos2d::Vec2 posLeft = bottomLeft + cocos2d::Vec2(0, y);
            const cocos2d::Vec2 posRight = bottomLeft + cocos2d::Vec2(ringWidth - 1, y);
            m_deadTiles.push_back(posLeft);
            m_deadTiles.push_back(posRight);
        }
    }
}

void GameModeBR::removeNextDeadTile()
{
    const cocos2d::Vec2 tile = m_deadTiles.back();
    m_deadTiles.pop_back();

    if (m_tileDeathCallback)
    {
        m_tileDeathCallback(tile.x, tile.y);
    }
}
