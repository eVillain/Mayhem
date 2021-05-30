#include "ParticlesController.h"

#include "Core/Dispatcher.h"
#include "SpawnParticlesEvent.h"
#include "GameView.h"
#include "Core/Injector.h"
#include "FollowNode.h"
#include "GameViewConstants.h"

ParticlesController::ParticlesController()
{
    printf("ParticlesController:: constructor: %p\n", this);
}

ParticlesController::~ParticlesController()
{
    printf("ParticlesController:: destructor: %p\n", this);
}

void ParticlesController::initialize()
{
    Dispatcher::globalDispatcher().addListener<SpawnParticlesEvent>(std::bind(&ParticlesController::onSpawnParticles,
                                                                              this, std::placeholders::_1),
                                                                    this);
    Dispatcher::globalDispatcher().addListener<SpawnParticlesAttachedEvent>(std::bind(&ParticlesController::onSpawnAttachedParticles,
                                                                                      this, std::placeholders::_1),
                                                                            this);
}

void ParticlesController::shutdown()
{
    Dispatcher::globalDispatcher().removeListener<SpawnParticlesEvent>(this);
    Dispatcher::globalDispatcher().removeListener<SpawnParticlesAttachedEvent>(this);
    
    m_attachedParticles.clear();
}

void ParticlesController::update()
{
    auto parentPair = m_attachedParticles.begin();
    for (; parentPair != m_attachedParticles.end();)
    {
        if (!parentPair->first->getParent())
        {
            for (const auto& particle : parentPair->second)
            {
                particle->removeFromParent();
            }
            parentPair = m_attachedParticles.erase(parentPair);
            continue;
        }
        parentPair++;
    }
}

void ParticlesController::onSpawnParticles(const SpawnParticlesEvent &event)
{
    auto particles = cocos2d::ParticleSystemQuad::create(event.getParticlesType());
    particles->setRotation(event.getRotation());
    particles->setPositionType(cocos2d::ParticleSystem::PositionType::RELATIVE);
    particles->setAutoRemoveOnFinish(true);
    cocos2d::Vec2 endPos = event.getEndPosition();
    if (endPos != cocos2d::Vec2::ZERO)
    {
        cocos2d::Vec2 startPos = event.getPosition();
        cocos2d::Vec2 dist = endPos - startPos;
        cocos2d::Vec2 pos = startPos + dist*0.5;
        cocos2d::Vec2 posVar = cocos2d::Vec2(dist.length()*0.5, particles->getPosVar().y);
        particles->setPosVar(posVar);
        particles->setPosition(pos);
    }
    else
    {
        particles->setPosition(event.getPosition());
    }

    auto gameView = Injector::globalInjector().getInstance<GameView>();
    if (event.getSelfLit())
    {
        gameView->getSelfLightingNode()->addChild(particles, GameViewConstants::Z_ORDER_GUN_MUZZLE);
    }
    else
    {
        gameView->getGameRootNode()->addChild(particles, GameViewConstants::Z_ORDER_GUN_MUZZLE);
    }
}

void ParticlesController::onSpawnAttachedParticles(const SpawnParticlesAttachedEvent& event)
{
    auto particles = cocos2d::ParticleSystemQuad::create(event.getParticlesType());
    particles->setRotation(event.getRotation());
    particles->setPositionType(event.getPositionType());
    particles->setPosition(event.getParent()->getPosition() + event.getOffset());
    particles->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
    particles->setAutoRemoveOnFinish(true);
    auto gameView = Injector::globalInjector().getInstance<GameView>();
    if (event.getSelfLit())
    {
        gameView->getSelfLightingNode()->addChild(particles, GameViewConstants::Z_ORDER_GUN_MUZZLE);
    }
    else
    {
        gameView->getGameRootNode()->addChild(particles, GameViewConstants::Z_ORDER_GUN_MUZZLE);
    }
    FollowNode* followNode = FollowNode::create(event.getParent(), event.getOffset());
    particles->runAction(followNode);
    
    auto it = m_attachedParticles.find(event.getParent());
    if (it != m_attachedParticles.end())
    {
        it->second.push_back(particles);
    }
    else
    {
        m_attachedParticles[event.getParent()].push_back(particles);
    }
}
