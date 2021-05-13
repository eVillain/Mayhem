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
                          const float speed)
{
    drawTimeLine();
    
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

void TimeLineView::drawTimeLine()
{
    m_drawNode->clear();

    const float TIMELINE_RIGHT = getContentSize().width - TIMELINE_PADDING;
    const float TIMELINE_WIDTH = TIMELINE_RIGHT - TIMELINE_PADDING;
    const float CURSOR_POS_X = TIMELINE_PADDING + ((m_currentTime / m_maxTime) * TIMELINE_WIDTH);
    const float CURSOR_TOP = TIMELINE_POS_Y - (CURSOR_HEIGHT * 0.5f);
    const float CURSOR_BOTTOM = TIMELINE_POS_Y + (CURSOR_HEIGHT * 0.5f);
    // Background
    m_drawNode->drawSolidRect(cocos2d::Vec2(TIMELINE_PADDING, CURSOR_BOTTOM), cocos2d::Vec2(TIMELINE_RIGHT, CURSOR_TOP), cocos2d::Color4F::BLACK);

    
    // Horizontal timeline
    m_drawNode->drawLine(cocos2d::Vec2(TIMELINE_PADDING, TIMELINE_POS_Y), cocos2d::Vec2(TIMELINE_RIGHT, TIMELINE_POS_Y), cocos2d::Color4F::GRAY);
    // Ends
    m_drawNode->drawLine(cocos2d::Vec2(TIMELINE_PADDING, TIMELINE_POS_Y-TIMELINE_ENDS_HEIGHT*0.5f), cocos2d::Vec2(TIMELINE_PADDING, TIMELINE_POS_Y+TIMELINE_ENDS_HEIGHT*0.5f), cocos2d::Color4F::GRAY);
    m_drawNode->drawLine(cocos2d::Vec2(TIMELINE_RIGHT, TIMELINE_POS_Y-TIMELINE_ENDS_HEIGHT*0.5f), cocos2d::Vec2(TIMELINE_RIGHT, TIMELINE_POS_Y+TIMELINE_ENDS_HEIGHT*0.5f), cocos2d::Color4F::GRAY);

    // Cursor
    m_drawNode->drawLine(cocos2d::Vec2(CURSOR_POS_X, CURSOR_TOP), cocos2d::Vec2(CURSOR_POS_X, CURSOR_BOTTOM), cocos2d::Color4F::GREEN);
}


