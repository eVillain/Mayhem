#ifndef LightModel_h
#define LightModel_h

#include "LightConstants.h"
#include "cocos2d.h"
#include <vector>

class LightModel
{
public:
    LightModel();
    
    const size_t addLight(const LightData& light);
    void removeLight(const size_t lightID);
        
    std::map<size_t, LightData>& getLights() { return m_lights; }
    const cocos2d::Color4F& getAmbienceColor() const { return m_ambienceColor; }
    void setAmbienceColor(cocos2d::Color4F ambienceColor) { m_ambienceColor = ambienceColor; }
    bool getApplyBlur() const { return m_applyBlur; }
    bool getDrawShadows() const { return m_drawShadows; }
    bool getDrawDebug() const { return m_drawDebug; }

    const size_t getNextLightID() { return s_nextLightID++; }
private:
    std::map<size_t, LightData> m_lights;
    cocos2d::Color4F m_ambienceColor;
    bool m_applyBlur;
    bool m_drawShadows;
    bool m_drawDebug;

    static size_t s_nextLightID;
};

#endif // LightModel_h
