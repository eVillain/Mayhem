#ifndef MAIN_MENU_VIEW_H
#define MAIN_MENU_VIEW_H

#include "cocos2d.h"
#include "ui/UIButton.h"

class EntityView;

class MainMenuView : public cocos2d::Layer
{
public:
    CREATE_FUNC(MainMenuView);
    
    MainMenuView();
    virtual ~MainMenuView();

    virtual bool init();

    cocos2d::Label* getTitle() const { return m_title; }
    cocos2d::Label* getPlayerName() const { return m_playerName; }
    cocos2d::ui::Button* getStartGameButton() const { return m_startGameButton; }
    cocos2d::ui::Button* getHostGameButton() const { return m_hostGameButton; }
    cocos2d::ui::Button* getJoinGameButton() const { return m_joinGameButton; }
    cocos2d::ui::Button* getSettingsGameButton() const { return m_settingsGameButton; }
    cocos2d::ui::Button* getReplayEditorButton() const { return m_replayEditorButton; }
    cocos2d::ui::Button* getExitGameButton() const { return m_exitGameButton; }

    cocos2d::Node* getPlayerNode() const { return m_playerNode; }
    const std::shared_ptr<EntityView>& getPlayerView() const { return m_playerView; }

private:
    cocos2d::Label* m_title;
    cocos2d::Label* m_playerName;
    cocos2d::ui::Button* m_startGameButton;
    cocos2d::ui::Button* m_hostGameButton;
    cocos2d::ui::Button* m_joinGameButton;
    cocos2d::ui::Button* m_settingsGameButton;
    cocos2d::ui::Button* m_replayEditorButton;
    cocos2d::ui::Button* m_exitGameButton;
    
    cocos2d::Node* m_playerNode;
    
    std::shared_ptr<EntityView> m_playerView;
};

#endif // MAIN_MENU_VIEW_H
