#ifndef TimeLineView_h
#define TimeLineView_h

#include "cocos2d.h"
#include "cocos-ext.h"

#include "NetworkMessages.h"

class TimeLineView : public cocos2d::Node
{
public:
    enum TimeLineButtonType {
        BUTTON_PLAY_PAUSE = 0,
        BUTTON_REWIND,
        BUTTON_FASTFORWARD,
        BUTTON_SPEED_PLUS,
        BUTTON_SPEED_MINUS,
    };
    
    TimeLineView();
    CREATE_FUNC(TimeLineView);
    
    void setContentSize(const cocos2d::Size& size) override;
    
    void update(const ssize_t currentFrame,
                const ssize_t totalFrames,
                const uint32_t tickRate,
                const float speed,
                const std::vector<SnapshotData>& snapshots);

    void setMinTime(float time) { m_minTime = time; }
    void setMaxTime(float time) { m_maxTime = time; }
    void setCurrentTime(float time) { m_currentTime = time; }
    
    float getMinTime() { return m_minTime; }
    float getMaxTime() { return m_maxTime; }
    float getCurrentTime() { return m_currentTime; }
    
    float getCursorTimeAtPosition(const cocos2d::Vec2& position) const;

    const std::vector<cocos2d::RefPtr<cocos2d::ui::Button>>& getButtons() const { return m_buttons; }

private:
    static const float TIMELINE_POS_Y;
    static const float TIMELINE_PADDING;
    static const float TIMELINE_ENDS_HEIGHT;
    static const float CURSOR_HEIGHT;
    static const float BUTTON_WIDTH;

    cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> m_background;
    cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> m_border;
    
    cocos2d::RefPtr<cocos2d::DrawNode> m_drawNode;
    
    cocos2d::RefPtr<cocos2d::Label> m_frameLabel;
    cocos2d::RefPtr<cocos2d::Label> m_timeLabel;
    
    std::vector<cocos2d::RefPtr<cocos2d::ui::Button>> m_buttons;

    float m_minTime;
    float m_maxTime;
    float m_currentTime;
    
    void drawTimeLine(const std::vector<SnapshotData>& snapshots,
                      const uint32_t tickRate);
};

#endif /* TimeLineView_h */
