#include "ReplayEditorView.h"
#include "Core/Dispatcher.h"
#include "HUDHelper.h"

ReplayEditorView::ReplayEditorView()
: m_levelView(LevelView::create())
, m_timeLine(TimeLineView::create())
{
    addChild(m_levelView);
    addChild(m_timeLine);
    
    auto winSize = cocos2d::Director::getInstance()->getWinSize();
    cocos2d::Size timeLineSize = cocos2d::Size(winSize.width, 100.f);

    m_timeLine->setContentSize(timeLineSize);
    m_timeLine->setPositionX(0.f);
}

