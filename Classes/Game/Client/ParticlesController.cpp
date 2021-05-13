#include "ParticlesController.h"
#include "Core/Dispatcher.h"
#include "SpawnParticlesEvent.h"
#include "GameView.h"
#include "Core/Injector.h"
#include "FollowNode.h"

ParticlesController::ParticlesController()
{
}

ParticlesController::~ParticlesController()
{
}

void ParticlesController::initialize()
{
    Dispatcher::globalDispatcher().addListener(SpawnParticlesEvent::descriptor, std::bind(&ParticlesController::onSpawnParticles, this, std::placeholders::_1));
    Dispatcher::globalDispatcher().addListener(SpawnParticlesAttachedEvent::descriptor, std::bind(&ParticlesController::onSpawnAttachedParticles, this, std::placeholders::_1));
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

void ParticlesController::onSpawnParticles(const Event &event)
{
    const SpawnParticlesEvent& spawnParticlesEvent = static_cast<const SpawnParticlesEvent&>(event);
    auto particles = cocos2d::ParticleSystemQuad::create(spawnParticlesEvent.getParticlesType());
    particles->setRotation(spawnParticlesEvent.getRotation());
    particles->setPositionType(cocos2d::ParticleSystem::PositionType::RELATIVE);
    particles->setAutoRemoveOnFinish(true);
    cocos2d::Vec2 endPos = spawnParticlesEvent.getEndPosition();
    if (endPos != cocos2d::Vec2::ZERO)
    {
        cocos2d::Vec2 startPos = spawnParticlesEvent.getPosition();
        cocos2d::Vec2 dist = endPos - startPos;
        cocos2d::Vec2 pos = startPos + dist*0.5;
        cocos2d::Vec2 posVar = cocos2d::Vec2(dist.length()*0.5, particles->getPosVar().y);
        particles->setPosVar(posVar);
        particles->setPosition(pos);
    }
    else
    {
        particles->setPosition(spawnParticlesEvent.getPosition());
    }

    auto gameView = Injector::globalInjector().getInstance<GameView>();
    if (spawnParticlesEvent.getSelfLit())
    {
        gameView->getSelfLightingNode()->addChild(particles, GameView::Z_ORDER_GUN_MUZZLE);
    }
    else
    {
        gameView->getGameRootNode()->addChild(particles, GameView::Z_ORDER_GUN_MUZZLE);
    }
}

void ParticlesController::onSpawnAttachedParticles(const Event& event)
{
    const SpawnParticlesAttachedEvent& spawnParticlesEvent = static_cast<const SpawnParticlesAttachedEvent&>(event);
    auto particles = cocos2d::ParticleSystemQuad::create(spawnParticlesEvent.getParticlesType());
    particles->setRotation(spawnParticlesEvent.getRotation());
    particles->setPositionType(spawnParticlesEvent.getPositionType());
    particles->setPosition(spawnParticlesEvent.getParent()->getPosition() + spawnParticlesEvent.getOffset());
    particles->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
    particles->setAutoRemoveOnFinish(true);
    auto gameView = Injector::globalInjector().getInstance<GameView>();
    if (spawnParticlesEvent.getSelfLit())
    {
        gameView->getSelfLightingNode()->addChild(particles, GameView::Z_ORDER_GUN_MUZZLE);
    }
    else
    {
        gameView->getGameRootNode()->addChild(particles, GameView::Z_ORDER_GUN_MUZZLE);
    }
    FollowNode* followNode = FollowNode::create(spawnParticlesEvent.getParent(), spawnParticlesEvent.getOffset());
    particles->runAction(followNode);
    
    auto it = m_attachedParticles.find(spawnParticlesEvent.getParent());
    if (it != m_attachedParticles.end())
    {
        it->second.push_back(particles);
    }
    else
    {
        m_attachedParticles[spawnParticlesEvent.getParent()].push_back(particles);
    }
}
