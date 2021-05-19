#include "LevelView.h"

#include "TogglePhysicsDebugEvent.h"
#include "Pseudo3DParticle.h"
#include "Pseudo3DSprite.h"
#include "HUDHelper.h"
#include "GameSettings.h"

LevelView::LevelView()
: m_tileMap(nullptr)
, m_bgTilesNode(nullptr)
, m_fgTilesNode(nullptr)
, m_metaTilesNode(nullptr)
, m_staticLightingNode(nullptr)
, m_selfLightingNode(nullptr)
{
}

void LevelView::setTileMap(const cocos2d::RefPtr<cocos2d::TMXTiledMap>& tileMap)
{
    if (m_tileMap)
    {
        removeChild(m_tileMap);
    }
    m_tileMap = tileMap;    
    addChild(m_tileMap);
    
    m_bgTilesNode = m_tileMap->getLayer("Background");
    m_fgTilesNode = m_tileMap->getLayer("Foreground");
    
    m_staticLightingNode = m_tileMap->getLayer("StaticLights");
    m_selfLightingNode = m_tileMap->getLayer("SelfLit");
    
    m_metaTilesNode = m_tileMap->getLayer("Meta");
}
