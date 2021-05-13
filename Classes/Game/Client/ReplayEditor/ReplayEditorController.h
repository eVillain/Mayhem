#ifndef ReplayEditorController_h
#define ReplayEditorController_h

#include "ReplayEditorView.h"

class GameView;
class LevelModel;
class ReplayModel;
class GameViewController;
class EntityInfoView;
class GameModel;

class ReplayEditorController
{
public:
    ReplayEditorController(std::shared_ptr<LevelModel> levelModel,
                           std::shared_ptr<ReplayModel> replayModel,
                           std::shared_ptr<GameModel> gameModel,
                           std::shared_ptr<GameView> gameView,
                           std::shared_ptr<GameViewController> gameViewController);
    
    void initialize();
    
    void update(const float deltaTime);

    const cocos2d::RefPtr<ReplayEditorView>& getView() const { return m_view; }

private:
    std::shared_ptr<LevelModel> m_levelModel;
    std::shared_ptr<ReplayModel> m_replayModel;
    std::shared_ptr<GameModel> m_gameModel;
    std::shared_ptr<GameView> m_gameView;
    std::shared_ptr<GameViewController> m_gameViewController;
    cocos2d::RefPtr<ReplayEditorView> m_view;
    EntityInfoView* m_infoView;
    cocos2d::EventListenerMouse* m_mouseListener;

    float m_playbackSpeed;
    size_t m_currentFrameIndex;
    bool m_isPlaying;
    bool m_isDraggingTimeLine;
    
    void setupMouseListener(cocos2d::EventDispatcher* dispatcher);

    void onMouseMoved(cocos2d::EventMouse* event);
    void onMouseDown(cocos2d::EventMouse* event);
    void onMouseUp(cocos2d::EventMouse* event);
    
    void onPlayPauseButton(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
    void onRewindButton(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
    void onFastForwardButton(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
    
    void onSpeedDownButton(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
    void onSpeedUpButton(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type);
    
    void jumpToMouseCoord(const cocos2d::Vec2& coord);
    void updatePlayButton();
    void updateView(const float time);
};


#endif /* ReplayEditorController_h */
