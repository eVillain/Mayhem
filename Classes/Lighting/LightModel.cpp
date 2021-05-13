#include "LightModel.h"

size_t LightModel::s_nextLightID = 0;

const cocos2d::Color4F DEFAULT_AMBIENT_COLOR = cocos2d::Color4F(0.05f, 0.05f, 0.1f, 0.0f);

LightModel::LightModel()
: m_applyBlur(false)
, m_drawShadows(true)
, m_drawDebug(false)
, m_ambienceColor(DEFAULT_AMBIENT_COLOR)
{
}

const size_t LightModel::addLight(const LightData& light)
{
    const size_t lightID = getNextLightID();
    m_lights[lightID] = light;
    return lightID;
}

void LightModel::removeLight(const size_t lightID)
{
    if (!m_lights.count(lightID))
    {
        return;
    }
    
    m_lights.erase(lightID);
}
