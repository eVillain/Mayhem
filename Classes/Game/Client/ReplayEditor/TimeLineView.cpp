#include "TimeLineView.h"

#include "HUDHelper.h"
#include "ButtonUtils.h"
#include "GameViewConstants.h"
#include "NumberFormatter.h"

const float TimeLineView::TIMELINE_POS_Y = 40.f;
const float TimeLineView::TIMELINE_PADDING = 8.f;
const float TimeLineView::TIMELINE_ENDS_HEIGHT = 8.f;
const float TimeLineView::CURSOR_HEIGHT = 20.f;
const float TimeLineView::BUTTON_WIDTH = 80.f;

TimeLineView::TimeLineView()
: m_background(nullptr)
, m_border(nullptr)
, m_drawNode(cocos2d::DrawNode::create())
, m_frameLabel(HUDHelper::createLabel3x6("Frame: ", 24))
, m_timeLabel(HUDHelper::createLabel3x6("Time: ", 24))
, m_minTime(0.f)
, m_maxTime(1.f)
, m_currentTime(0.f)
{
    auto box = HUDHelper::createBox(cocos2d::Size(BUTTON_WIDTH*5+TIMELINE_PADDING*2, 100.f),
                                    cocos2d::Vec2::ZERO);
    m_background = box.fill;
    m_border = box.border;
    
    addChild(m_background);
    addChild(m_border);
    addChild(m_drawNode);
    
    addChild(m_frameLabel);
    addChild(m_timeLabel);
    
    for (int i = 0; i < 5; i++)
    {
        cocos2d::Vec2 buttonPos = cocos2d::Vec2(8.f + BUTTON_WIDTH*i, 8.f);
        auto button = ButtonUtils::createButton(buttonPos,
                                                cocos2d::Size(BUTTON_WIDTH, 20.f),
                                                "Play",
                                                GameViewConstants::FONT_SIZE_MEDIUM);
        button->setAnchorPoint(cocos2d::Vec2::ZERO);
        button->setTag(i);
        addChild(button);
        m_buttons.push_back(button);
    }
    m_buttons[TimeLineButtonType::BUTTON_PLAY_PAUSE]->setTitleText("Play");
    m_buttons[TimeLineButtonType::BUTTON_REWIND]->setTitleText("<<RW");
    m_buttons[TimeLineButtonType::BUTTON_FASTFORWARD]->setTitleText("FF>>");
    m_buttons[TimeLineButtonType::BUTTON_SPEED_PLUS]->setTitleText("+");
    m_buttons[TimeLineButtonType::BUTTON_SPEED_MINUS]->setTitleText("-");
}

void TimeLineView::setContentSize(const cocos2d::Size& size)
{
    Node::setContentSize(size);
    
    m_background->setPreferredSize(size);
    m_border->setPreferredSize(size - cocos2d::Size(2, 2));
    
    m_frameLabel->setPosition(cocos2d::Vec2(TIMELINE_PADDING, getContentSize().height - 20));
    m_frameLabel->setAnchorPoint(cocos2d::Vec2::ZERO);
    m_timeLabel->setPosition(cocos2d::Vec2(TIMELINE_PADDING, getContentSize().height - 40));
    m_timeLabel->setAnchorPoint(cocos2d::Vec2::ZERO);
    
    for (int i = 0; i < 5; i++)
    {
        cocos2d::Vec2 buttonPos = cocos2d::Vec2(8.f + BUTTON_WIDTH*i, 8.f);
        m_buttons.at(i)->setPosition(buttonPos);
    }
}

void TimeLineView::update(const ssize_t currentFrame,
                          const ssize_t totalFrames,
                          const uint32_t tickRate,
                          const float speed,
                          const std::vector<SnapshotData>& snapshots)
{
    drawTimeLine(snapshots, tickRate);
    
    const std::string frame = "Frame: " + std::to_string(currentFrame) + " / " + std::to_string(totalFrames) + " TickRate: " + std::to_string(tickRate) + "Hz";
    m_frameLabel->setString(frame);
    const std::string time = "Time: " + NumberFormatter::toString(m_currentTime) + " / " + NumberFormatter::toString(m_maxTime) + " Speed: " + NumberFormatter::toString(speed) + "x";
    m_timeLabel->setString(time);
}

float TimeLineView::getCursorTimeAtPosition(const cocos2d::Vec2& position) const
{
    if (position.x < 0.f || position.x > getContentSize().width ||
        position.y < TIMELINE_POS_Y - CURSOR_HEIGHT*0.5f || position.y > TIMELINE_POS_Y + CURSOR_HEIGHT*0.5f)
    {
        return -1;
    }
    
    const float TIMELINE_RIGHT = getContentSize().width - TIMELINE_PADDING;
    const float TIMELINE_WIDTH = TIMELINE_RIGHT - TIMELINE_PADDING;

    const float timeRatio = (position.x - TIMELINE_PADDING) / TIMELINE_WIDTH;
    const float time = std::max(std::min(timeRatio, 1.f), 0.f) * m_maxTime;
    return time;
}

void TimeLineView::drawTimeLine(const std::vector<SnapshotData>& snapshots,
                                const uint32_t tickRate)
{
    m_drawNode->clear();

    const float TIMELINE_RIGHT = getContentSize().width - TIMELINE_PADDING;
    const float TIMELINE_WIDTH = TIMELINE_RIGHT - TIMELINE_PADDING;
    const float CURSOR_POS_X = TIMELINE_PADDING + ((m_currentTime / m_maxTime) * TIMELINE_WIDTH);
    const float CURSOR_BOTTOM = TIMELINE_POS_Y - (CURSOR_HEIGHT * 0.5f);
    const float CURSOR_TOP = TIMELINE_POS_Y + (CURSOR_HEIGHT * 0.5f);
    // Background
    m_drawNode->drawSolidRect(cocos2d::Vec2(TIMELINE_PADDING, CURSOR_BOTTOM), cocos2d::Vec2(TIMELINE_RIGHT, CURSOR_TOP), cocos2d::Color4F::BLACK);

    // Horizontal timeline
    m_drawNode->drawLine(cocos2d::Vec2(TIMELINE_PADDING, TIMELINE_POS_Y), cocos2d::Vec2(TIMELINE_RIGHT, TIMELINE_POS_Y), cocos2d::Color4F::GRAY);
    // Ends
    m_drawNode->drawLine(cocos2d::Vec2(TIMELINE_PADDING, TIMELINE_POS_Y-TIMELINE_ENDS_HEIGHT*0.5f), cocos2d::Vec2(TIMELINE_PADDING, TIMELINE_POS_Y+TIMELINE_ENDS_HEIGHT*0.5f), cocos2d::Color4F::GRAY);
    m_drawNode->drawLine(cocos2d::Vec2(TIMELINE_RIGHT, TIMELINE_POS_Y-TIMELINE_ENDS_HEIGHT*0.5f), cocos2d::Vec2(TIMELINE_RIGHT, TIMELINE_POS_Y+TIMELINE_ENDS_HEIGHT*0.5f), cocos2d::Color4F::GRAY);

    // Cursor
    m_drawNode->drawLine(cocos2d::Vec2(CURSOR_POS_X, CURSOR_TOP), cocos2d::Vec2(CURSOR_POS_X, CURSOR_BOTTOM), cocos2d::Color4F::GREEN);
    
    // Zoomed-in detail view of frames
    const float DETAIL_WIDTH = 400.f;
    const float DETAIL_HEIGHT = 30.f;
    const float DETAIL_POS_X = TIMELINE_RIGHT - DETAIL_WIDTH;
    const float DETAIL_POS_Y = CURSOR_TOP + TIMELINE_PADDING;
    m_drawNode->drawRect(cocos2d::Vec2(DETAIL_POS_X,
                                       DETAIL_POS_Y),
                        cocos2d::Vec2(DETAIL_POS_X + DETAIL_WIDTH,
                                       DETAIL_POS_Y + DETAIL_HEIGHT),
                        cocos2d::Color4F::BLACK);
    
    // Detail view cursor
    m_drawNode->drawLine(cocos2d::Vec2(DETAIL_POS_X + DETAIL_WIDTH*0.5f, DETAIL_POS_Y),
                         cocos2d::Vec2(DETAIL_POS_X + DETAIL_WIDTH*0.5f, DETAIL_POS_Y + DETAIL_HEIGHT),
                         cocos2d::Color4F::GREEN);

    const uint32_t VISIBLE_FRAMES = 40;
    const uint32_t VISIBLE_RANGE = VISIBLE_FRAMES/2;

    const float FRAME_WIDTH = DETAIL_WIDTH / VISIBLE_FRAMES;
    const float FRAME_HEIGHT = DETAIL_HEIGHT - (TIMELINE_PADDING * 2);
    const uint32_t targetFrame = std::floor(m_currentTime * tickRate);
    const uint32_t firstVisibleFrame = targetFrame >= VISIBLE_RANGE ? targetFrame - VISIBLE_RANGE : 0;
    const uint32_t lastVisibleFrame = targetFrame > snapshots.size() - VISIBLE_RANGE ? (uint32_t)snapshots.size() : targetFrame + VISIBLE_RANGE;

    const float frameTime = 1.f / tickRate;
    const float frameStartTime = targetFrame * frameTime;
    const float alphaTime = std::min(std::max((m_currentTime - frameStartTime) / frameTime, 0.f), 1.f);
    const float offsetX = -alphaTime * FRAME_WIDTH;
    const float posY = DETAIL_POS_Y + TIMELINE_PADDING;

    for (const auto& snapshot : snapshots)
    {
        if (snapshot.serverTick < firstVisibleFrame ||
            snapshot.serverTick > lastVisibleFrame)
        {
            continue;
        }

        const float posX = DETAIL_POS_X + offsetX + ((snapshot.serverTick - (targetFrame - VISIBLE_RANGE)) * FRAME_WIDTH);
        const float alpha = 1.f-fabs((float(snapshot.serverTick) - targetFrame) / VISIBLE_RANGE);
        
        cocos2d::Color4F color = (snapshot.serverTick % tickRate == 0) ? cocos2d::Color4F::WHITE : cocos2d::Color4F::GRAY;
        color.a = alpha;
        m_drawNode->drawRect(cocos2d::Vec2(posX, posY),
                             cocos2d::Vec2(posX + FRAME_WIDTH - 1, posY + FRAME_HEIGHT),
                             color);
        
        if (!snapshot.hitData.empty())
        {
            m_drawNode->drawSolidRect(cocos2d::Vec2(posX+1, posY+1),
                                      cocos2d::Vec2(posX + FRAME_WIDTH - 2, posY + FRAME_HEIGHT - 1),
                                      cocos2d::Color4F(1.f, 0.f, 0.f, alpha));

        }
    }
}


