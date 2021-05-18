#include "GameModeDM.h"
#include "Game/Shared/EntityConstants.h"

#include "EntitiesModel.h"
#include "EntitiesController.h"
#include "LevelModel.h"
#include "EntityDataModel.h"

GameModeDM::GameModeDM(std::shared_ptr<EntitiesController> entitiesController,
           std::shared_ptr<EntitiesModel> entitiesModel,
           std::shared_ptr<LevelModel> levelModel,
           const int seed)
: GameMode(entitiesController, entitiesModel, levelModel)
, m_seed(seed)
, m_maxKills(10)
, m_maxTime(5.f * 60.f)
, m_time(0.f)
{
}

void GameModeDM::update(const float deltaTime)
{
    m_time += deltaTime;
    if (m_time >= m_maxTime)
    {
        if (m_winConditionCallback)
        {
            uint32_t mostKills = 0;
            uint32_t playerWithMostKillsID = 0;
            for (const auto& pair : m_playerKills)
            {
                if (mostKills < pair.second)
                {
                    mostKills = pair.second;
                    playerWithMostKillsID = pair.first;
                }
            }
            m_winConditionCallback(playerWithMostKillsID);
        }
    }
}

void GameModeDM::onLevelLoaded(const bool isHost)
{
    if (isHost)
    {
        spawnThings();
    }
}

void GameModeDM::onPlayerGotAKill(const uint8_t playerID)
{
    if (m_playerKills.find(playerID) == m_playerKills.end())
    {
        m_playerKills[playerID] = 1;
        return;
    }
    m_playerKills[playerID]++;
    
    if (m_playerKills.at(playerID) >= m_maxKills)
    {
        if (m_winConditionCallback)
        {
            m_winConditionCallback(playerID);
        }
    }
}

void GameModeDM::spawnThings()
{
    const bool SPAWN_RANDOM_WEAPONS = true;
    if (SPAWN_RANDOM_WEAPONS)
    {
        const cocos2d::Size scaling = m_levelModel->getTileMap()->getTileSize();
        const float CHUNK_SIZE = 16.f;
        int xChunks = m_levelModel->getTileMap()->getMapSize().width / CHUNK_SIZE;
        int yChunks = m_levelModel->getTileMap()->getMapSize().height / CHUNK_SIZE;

        for (int x = 0; x < xChunks; x++)
        {
            for (int y = 0; y < yChunks; y++)
            {
                const cocos2d::Vec2 random = cocos2d::Vec2(cocos2d::random(-4, 4), cocos2d::random(-4, 4)) * 16.f;
                const cocos2d::Vec2 chunkMiddle = cocos2d::Vec2(CHUNK_SIZE * 0.5f, CHUNK_SIZE * 0.5f);
                const cocos2d::Vec2 position = (((cocos2d::Vec2(x, y) * CHUNK_SIZE) + chunkMiddle) * scaling.width) + random;
                spawnRandomGunWithAmmo(position);
                
            }
        }
    }
    else
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
}

void GameModeDM::spawnRandomGunWithAmmo(const cocos2d::Vec2& position)
{
    // TODO: Save in weapon cluster data for respawning after being picked up
    const int randomWeapon = cocos2d::random((int)EntityType::Item_First_Placeholder + 1, (int)EntityType::Item_Railgun);
    m_entitiesController->createItem(m_entitiesModel->getNextEntityID(),
                                     (EntityType)randomWeapon,
                                     position,
                                     0.f);
    m_entitiesModel->incrementNextEntityID();
    
    const auto& itemData = EntityDataModel::getStaticEntityData((EntityType)randomWeapon);
        
    const int randomAmmoCount = cocos2d::random(1, 3);
    for (int i = 0; i < randomAmmoCount; i++)
    {
        const cocos2d::Vec2 random = cocos2d::Vec2(cocos2d::random(-16, 16), cocos2d::random(-16, 16));

        m_entitiesController->createItem(m_entitiesModel->getNextEntityID(),
                                     (EntityType)itemData.ammo.type,
                                     position + random,
                                     0.f);
        m_entitiesModel->incrementNextEntityID();
    }
}
