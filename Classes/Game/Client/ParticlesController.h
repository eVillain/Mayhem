#ifndef ParticlesController_h
#define ParticlesController_h

#include "cocos2d.h"

class ParticlesModel;
class SpawnParticlesEvent;
class SpawnParticlesAttachedEvent;

class ParticlesController
{
public:
    ParticlesController();
    ~ParticlesController();
    
    void initialize();
    void shutdown();

    void update();
    
private:
    void onSpawnParticles(const SpawnParticlesEvent& event);
    void onSpawnAttachedParticles(const SpawnParticlesAttachedEvent& event);
    
    std::map<cocos2d::RefPtr<cocos2d::Node>, std::vector<cocos2d::RefPtr<cocos2d::ParticleSystemQuad>>> m_attachedParticles;
};

#endif /* Particles_h */
