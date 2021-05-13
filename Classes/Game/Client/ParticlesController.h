#ifndef ParticlesController_h
#define ParticlesController_h

#include "cocos2d.h"

class Event;

class ParticlesModel;

class ParticlesController
{
public:
    ParticlesController();
    ~ParticlesController();
    
    void initialize();
    void update();
    
private:
    
    void onSpawnParticles(const Event& event);
    void onSpawnAttachedParticles(const Event& event);
    
    std::map<cocos2d::RefPtr<cocos2d::Node>, std::vector<cocos2d::RefPtr<cocos2d::ParticleSystemQuad>>> m_attachedParticles;
};

#endif /* Particles_h */
