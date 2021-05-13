#ifndef MAIN_MENU_VIEW_H
#define MAIN_MENU_VIEW_H

#include "cocos2d.h"
#include "ui/UIButton.h"

class MainMenuView : public cocos2d::Layer
{
public:
    virtual bool init();
    
    CREATE_FUNC(MainMenuView);
    
    cocos2d::ui::Button* getStartGameButton() { return m_startGameButton; }
    cocos2d::ui::Button* getHostGameButton() { return m_hostGameButton; }
    cocos2d::ui::Button* getJoinGameButton() { return m_joinGameButton; }
    cocos2d::ui::Button* getSettingsGameButton() { return m_settingsGameButton; }
    cocos2d::ui::Button* getReplayEditorButton() { return m_replayEditorButton; }
    cocos2d::ui::Button* getExitGameButton() { return m_exitGameButton; }

private:
    cocos2d::Label* m_title;
    cocos2d::ui::Button* m_startGameButton;
    cocos2d::ui::Button* m_hostGameButton;
    cocos2d::ui::Button* m_joinGameButton;
    cocos2d::ui::Button* m_settingsGameButton;
    cocos2d::ui::Button* m_replayEditorButton;
    cocos2d::ui::Button* m_exitGameButton;
};

#endif // MAIN_MENU_VIEW_H
