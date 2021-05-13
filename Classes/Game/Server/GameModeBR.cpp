#include "GameModeBR.h"
#include "Game/Shared/EntityConstants.h"

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
    const float DEAD_TILE_REMOVE_PERIOD = 0.1f;
    m_tileUpdateAccumulator += deltaTime;
    
    while (m_tileUpdateAccumulator > DEAD_TILE_REMOVE_PERIOD)
    {
        m_tileUpdateAccumulator -= DEAD_TILE_REMOVE_PERIOD;
        
        if (m_deadTiles.empty())
        {
            fillDeadTiles();
        }
        
        removeNextDeadTile();
    }
}

void GameModeBR::onLevelLoaded(const bool isHost)
{
    if (isHost)
    {
        spawnThings();
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
//    const cocos2d::Size mapSize = m_tileMap->getMapSize();
//    const int rightRow = (mapSize.width - 1) - m_tileUpdateLayer;
//    const int topRow = (mapSize.height - 1) - m_tileUpdateLayer;
//    for (int x = m_tileUpdateLayer; x < rightRow; x++) {
//        m_deadTiles.push_back(cocos2d::Vec2(x, m_tileUpdateLayer)); // Top row tile
//        m_deadTiles.push_back(cocos2d::Vec2(x, topRow)); // Bottom row tile
//    }
//    for (int y = m_tileUpdateLayer; y < topRow; y++) {
//        m_deadTiles.push_back(cocos2d::Vec2(m_tileUpdateLayer, y)); // Left row tile
//        m_deadTiles.push_back(cocos2d::Vec2(rightRow, y)); // Right row tile
//    }
//    m_tileUpdateLayer++;
}

void GameModeBR::removeNextDeadTile()
{
//    const int randomIndex = cocos2d::random(0, (int)(m_deadTiles.size() - 1));
//    const cocos2d::Vec2 tile = m_deadTiles.at(randomIndex);
    
//    for (const auto& tile : m_deadTiles)
//    {
//        cocos2d::TMXLayer* foreground = m_tileMap->getLayer("Foreground");
//        cocos2d::TMXLayer* background = m_tileMap->getLayer("Background");
//        cocos2d::Sprite* foregroundSprite = foreground->getTileAt(tile);
//        cocos2d::Sprite* backgroundSprite = background->getTileAt(tile);
//
//        if (foregroundSprite)
//        {
//            foregroundSprite->runAction(cocos2d::FadeOut::create(1.f));
//        }
//        if (backgroundSprite)
//        {
//            backgroundSprite->runAction(cocos2d::FadeOut::create(1.f));
//        }
//        if (!foregroundSprite && !backgroundSprite)
//        {
//            printf("fail\n");
//        }
//    }
    
//    m_deadTiles.clear();

//    m_deadTiles.erase(m_deadTiles.begin() + randomIndex);
}
