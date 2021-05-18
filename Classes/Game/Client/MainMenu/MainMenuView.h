#ifndef MAIN_MENU_VIEW_H
#define MAIN_MENU_VIEW_H

#include "cocos2d.h"
#include "ui/UIButton.h"

class MainMenuView : public cocos2d::Layer
{
public:
    virtual bool init();
    
    CREATE_FUNC(MainMenuView);
    
    cocos2d::Label* getTitle() const { return m_title; }
    cocos2d::Label* getPlayerName() const { return m_playerName; }
    cocos2d::ui::Button* getStartGameButton() const { return m_startGameButton; }
    cocos2d::ui::Button* getHostGameButton() const { return m_hostGameButton; }
    cocos2d::ui::Button* getJoinGameButton() const { return m_joinGameButton; }
    cocos2d::ui::Button* getSettingsGameButton() const { return m_settingsGameButton; }
    cocos2d::ui::Button* getReplayEditorButton() const { return m_replayEditorButton; }
    cocos2d::ui::Button* getExitGameButton() const { return m_exitGameButton; }

private:
    cocos2d::Label* m_title;
    cocos2d::Label* m_playerName;
    cocos2d::ui::Button* m_startGameButton;
    cocos2d::ui::Button* m_hostGameButton;
    cocos2d::ui::Button* m_joinGameButton;
    cocos2d::ui::Button* m_settingsGameButton;
    cocos2d::ui::Button* m_replayEditorButton;
    cocos2d::ui::Button* m_exitGameButton;
};

#endif // MAIN_MENU_VIEW_H
