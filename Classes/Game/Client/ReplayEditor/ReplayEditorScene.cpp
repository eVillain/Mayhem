#include "ReplayEditorScene.h"
#include "Core/Injector.h"
#include "ReplayEditorController.h"
#include "ReplayEditorView.h"
#include "LevelModel.h"

USING_NS_CC;

Scene* ReplayEditorScene::createScene()
{
    return ReplayEditorScene::create();
}

// on "init" you need to initialize your instance
bool ReplayEditorScene::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Scene::init())
    {
        return false;
    }    
    
    return true;
}

void ReplayEditorScene::onEnter()
{
    cocos2d::Scene::onEnter();
    scheduleUpdate();
    
//    auto scheduler = cocos2d::Director::getInstance()->getScheduler();
//    scheduler->setTimeScale(0.1f);
}

void ReplayEditorScene::onExit()
{
    cocos2d::Scene::onExit();
    unscheduleUpdate();
}

void ReplayEditorScene::update(float deltaTime)
{
    m_controller->update(deltaTime);
}

void ReplayEditorScene::setController(std::shared_ptr<ReplayEditorController> controller)
{
    m_controller = controller;
}
