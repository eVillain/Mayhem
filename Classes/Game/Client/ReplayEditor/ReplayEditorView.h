#ifndef ReplayEditorView_h
#define ReplayEditorView_h

#include "cocos2d.h"
#include "cocos-ext.h"
#include "TimeLineView.h"
#include "LevelView.h"
#include "GameView.h"

class ReplayEditorView : public cocos2d::Node
{
public:
    ReplayEditorView();
    CREATE_FUNC(ReplayEditorView);

    const cocos2d::RefPtr<LevelView>& getLevelView() const { return m_levelView; }
    const GameView* getGameView() const { return m_gameView; }
    const cocos2d::RefPtr<TimeLineView>& getTimeLineView() const { return m_timeLine; }

private:
    cocos2d::RefPtr<LevelView> m_levelView;
    GameView* m_gameView;
    cocos2d::RefPtr<TimeLineView> m_timeLine;
};

#endif /* ReplayEditorView_h */
