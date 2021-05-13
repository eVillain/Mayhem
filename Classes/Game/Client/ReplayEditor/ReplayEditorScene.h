#ifndef __REPLAY_EDITOR_SCENE_H__
#define __REPLAY_EDITOR_SCENE_H__

#include "cocos2d.h"

class ReplayEditorController;

class ReplayEditorScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    bool init() override;
    
    void onEnter() override;
    void onExit() override;

    void update(float deltaTime) override;
    
    void setController(std::shared_ptr<ReplayEditorController> controller);
    
    // implement the "static create()" method manually
    CREATE_FUNC(ReplayEditorScene);
    
private:
    std::shared_ptr<ReplayEditorController> m_controller;
};

#endif // __REPLAY_EDITOR_SCENE_H__
