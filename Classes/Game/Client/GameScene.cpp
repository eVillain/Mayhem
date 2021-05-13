#include "GameScene.h"
#include "Game/Client/ClientController.h"
#include "Game/Server/ServerController.h"
#include "Core/Injector.h"

// Only for debugging
#include "Game/Client/SnapshotModel.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
    return GameScene::create();
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Scene::init())
    {
        return false;
    }
    
    auto& injector = Injector::globalInjector();
    m_clientController = injector.getInstance<ClientController>();
    if (injector.hasMapping<ServerController>())
    {
        m_serverController = injector.getInstance<ServerController>();
    }
    
    return true;
}

void GameScene::onEnter()
{
    cocos2d::Scene::onEnter();
    scheduleUpdate();
    
//    auto scheduler = cocos2d::Director::getInstance()->getScheduler();
//    scheduler->setTimeScale(0.1f);
}

void GameScene::onExit()
{
    cocos2d::Scene::onExit();
    unscheduleUpdate();
}

void GameScene::update(float deltaTime)
{
    m_clientController->update(deltaTime);

    if (m_serverController)
    {
        m_serverController->update(deltaTime);
        
//            bool compareSnapshots = true; // Debug to see if local prediction matches truth on server
//            if (compareSnapshots)
//            {
//                const auto& snapshotModel = Injector::globalInjector().getInstance<SnapshotModel>();
//                const float targetTime = m_clientController->getCurrentTime() - m_clientController->getInterpolationLatency();
//                size_t targetTimeSnapshotIndex = snapshotModel->getSnapshotIndexForTime(targetTime);
//                auto clientSnapshot = snapshotModel->getSnapshotIndexForTime(targetTimeSnapshotIndex);
        //        auto serverSnapshot = m_serverController->get
//                
//            }
    }
}
