#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

class ClientController;
class ServerController;

class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    bool init() override;
    
    void onEnter() override;
    void onExit() override;

    void update(float deltaTime) override;
        
    // implement the "static create()" method manually
    CREATE_FUNC(GameScene);
    
private:
    std::shared_ptr<ClientController> m_clientController;
    std::shared_ptr<ServerController> m_serverController;
};

#endif // __GAME_SCENE_H__
