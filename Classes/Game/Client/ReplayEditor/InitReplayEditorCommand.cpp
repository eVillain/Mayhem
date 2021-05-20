#include "InitReplayEditorCommand.h"

#include "AudioController.h"
#include "AudioModel.h"
#include "ClientModel.h"
#include "cocos2d.h"
#include "Core/Injector.h"
#include "EntityDataModel.h"
#include "GameModel.h"
#include "GameSettings.h"
#include "GameView.h"
#include "GameViewController.h"
#include "HUDView.h"
#include "InputController.h"
#include "InputModel.h"
#include "LevelModel.h"
#include "LightController.h"
#include "LightModel.h"
#include "LoadLevelCommand.h"
#include "LoadStaticEntityDataCommand.h"
#include "ParticlesController.h"
#include "ReplayEditorController.h"
#include "ReplayEditorScene.h"
#include "ReplayModel.h"

bool InitReplayEditorCommand::run()
{
    LoadLevelCommand loadLevel("BitTileMap.tmx");
    loadLevel.run();

    if (!EntityDataModel::hasStaticEntityData())
    {
        LoadStaticEntityDataCommand loadStaticEntityData;
        loadStaticEntityData.run();
        EntityDataModel::setStaticEntityData(loadStaticEntityData.itemDataMap);
    }
    
    Injector& injector = Injector::globalInjector();

    auto levelModel = injector.getInstance<LevelModel>();
    
    if (!injector.hasMapping<ReplayModel>())
    {
        injector.mapSingleton<ReplayModel>();
    }
    if (!injector.hasMapping<GameModel>())
    {
        injector.mapSingleton<GameModel>();
    }
    if (!injector.hasMapping<GameView>())
    {
        injector.mapSingleton<GameView, GameSettings>();
    }
    if (!injector.hasMapping<ParticlesController>())
    {
        injector.mapSingleton<ParticlesController>();
    }
    if (!injector.hasMapping<AudioModel>())
    {
        injector.mapSingleton<AudioModel>();
    }
    if (!injector.hasMapping<AudioController>())
    {
        injector.mapSingleton<AudioController,
            AudioModel,
            GameSettings>();
    }
    if (!injector.hasMapping<HUDView>())
    {
        injector.mapSingleton<HUDView>();
    }
    
    if (!injector.hasMapping<InputController>())
    {
        if (!injector.hasMapping<InputModel>())
        {
            injector.mapSingleton<InputModel>();
        }
        injector.mapSingleton<InputController,
            InputModel>();
    }
    
    if (!injector.hasMapping<LightController>())
    {
        if (!injector.hasMapping<LightModel>())
        {
            injector.mapSingleton<LightModel>();
        }
        injector.mapSingleton<LightController,
            LightModel, GameSettings>();
        auto lightController = injector.getInstance<LightController>();
        lightController->initialize();
    }
    
    if (!injector.hasMapping<GameViewController>())
    {
        injector.mapSingleton<GameViewController,
            GameSettings,
            LevelModel,
            GameView,
            InputModel,
            LightController,
            ParticlesController,
            HUDView,
            AudioController>();
    }
    
    auto hudView = injector.getInstance<HUDView>();
    hudView->initialize();
    
    auto gameView = injector.getInstance<GameView>();
    gameView->initialize();
    gameView->setTileMap(levelModel->getTileMap());
    
    auto particlesController = injector.getInstance<ParticlesController>();
    particlesController->initialize();
    auto audioController = injector.getInstance<AudioController>();
    audioController->initialize();
    
    auto controller = injector.instantiateUnmapped<ReplayEditorController,
        LevelModel, ReplayModel, GameModel, GameView, GameViewController>();
    controller->initialize();
    
    auto replayEditorScene = ReplayEditorScene::create();
    replayEditorScene->setController(controller);
    
    auto inputController = injector.getInstance<InputController>();
    inputController->initialize(replayEditorScene);
        
    // Add game views to scene
    auto gameSettings = injector.getInstance<GameSettings>();
    const cocos2d::Value& deferredRenderSetting = gameSettings->getValue(GameView::SETTING_RENDER_DEFERRED, cocos2d::Value(true));
    if (deferredRenderSetting.asBool())
    {
        replayEditorScene->addChild(gameView->getRenderTexture(), -1);
    }
    
    replayEditorScene->addChild(gameView->getView());

//        const cocos2d::Value& renderLightingSetting = gameSettings->getValue(LightController::SETTING_RENDER_LIGHTING, cocos2d::Value(true));
//        if (renderLightingSetting.asBool())
//        {
//            // The actual lightmap, this needs to be added here for lighting to work
//            const cocos2d::Value& postProcessSetting = gameSettings->getValue(GameView::SETTING_RENDER_POSTPROCESS, cocos2d::Value(true));
//            if (!postProcessSetting.asBool())
//            {
//                // No post-processing, apply lighting directly on top of game scene
//                replayEditorScene->addChild(lightController->getLightMapTexture(), 2);
//            }
//            gameView->getSelfLightingNode()->removeFromParent();
//            gameScene->addChild(gameView->getSelfLightingNode(), 3);
//        }
        
//        replayEditorScene->addChild(hudView->getRoot(), 3);
    replayEditorScene->addChild(controller->getView(), 3);
    replayEditorScene->addChild(gameView->getDebugDrawNode(), 10);
    replayEditorScene->addChild(gameView->getDebugLabelNode(), 11);
    
    // run
    if (!cocos2d::Director::getInstance()->getRunningScene())
    {
        cocos2d::Director::getInstance()->runWithScene(replayEditorScene);
    }
    else
    {
        cocos2d::Director::getInstance()->replaceScene(replayEditorScene);
    }
    
    return true;
}
