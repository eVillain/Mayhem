#ifndef MAIN_MENU_CONTROLLER_H
#define MAIN_MENU_CONTROLLER_H

#include "cocos2d.h"
#include "MainMenuView.h"
#include "NetworkHostView.h"
#include "NetworkClientView.h"

class NetworkController;
class BackToMainMenuEvent;
class InputActionEvent;

class MainMenuController : public cocos2d::Scene
{
public:
    MainMenuController();
    virtual ~MainMenuController();

    virtual bool init() override;
    void shutdown();
    CREATE_FUNC(MainMenuController);

    virtual void update(float deltaTime) override;

    void openNetworkView();
    
private:
    cocos2d::RefPtr<MainMenuView> m_view;
    cocos2d::RefPtr<NetworkHostView> m_networkHostView;
    cocos2d::RefPtr<NetworkClientView> m_networkClientView;
    
    const std::string getPlayerName() const;
    
    void startGameCallback(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
    void hostGameCallback(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
    void joinGameCallback(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
    void settingsCallback(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
    void replayEditorCallback(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
    void exitGameCallback(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);

    void onBackToMainMenu(const BackToMainMenuEvent& event);
    void onInputAction(const InputActionEvent& event);
    void exitGame();
};

#endif /* MAIN_MENU_CONTROLLER_H */
