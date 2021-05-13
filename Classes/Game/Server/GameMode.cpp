#include "GameMode.h"

GameMode::GameMode(std::shared_ptr<EntitiesController> entitiesController,
                   std::shared_ptr<EntitiesModel> entitiesModel,
                   std::shared_ptr<LevelModel> levelModel)
: m_entitiesController(entitiesController)
, m_entitiesModel(entitiesModel)
, m_levelModel(levelModel)
{
}

