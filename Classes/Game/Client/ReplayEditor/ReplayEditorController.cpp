#include "ReplayEditorController.h"

#include "ReplayEditorView.h"
#include "LevelModel.h"
#include "ReplayModel.h"
#include "TimeLineView.h"
#include "EntityInfoView.h"
#include "SharedConstants.h"
#include "GameViewController.h"
#include "PlayerLogic.h"
#include "EntityDataModel.h"
#include "Utils/NumberFormatter.h"
#include "GameModel.h"

ReplayEditorController::ReplayEditorController(std::shared_ptr<LevelModel> levelModel,
                                               std::shared_ptr<ReplayModel> replayModel,
                                               std::shared_ptr<GameModel> gameModel,
                                               std::shared_ptr<GameView> gameView,
                                               std::shared_ptr<GameViewController> gameViewController)
: m_levelModel(levelModel)
, m_replayModel(replayModel)
, m_gameModel(gameModel)
, m_gameView(gameView)
, m_gameViewController(gameViewController)
, m_view(ReplayEditorView::create())
, m_infoView(nullptr)
, m_mouseListener(nullptr)
, m_playbackSpeed(1.f)
, m_currentFrameIndex(-1)
, m_isPlaying(false)
, m_isDraggingTimeLine(false)
{
}

void ReplayEditorController::initialize()
{
    m_replayModel->loadFile(ReplayModel::DEFAULT_REPLAY_FILE);
    
    m_gameModel->setTickRate(m_replayModel->getTickRate());
    
    const auto& snapshots = m_replayModel->getSnapshots();
    const float maxTime = snapshots.back().serverTick * m_gameModel->getFrameTime();
    m_view->getTimeLineView()->setCurrentTime(0.f);
    m_view->getTimeLineView()->setMaxTime(maxTime);

    auto playButton = m_view->getTimeLineView()->getButtons().at(TimeLineView::TimeLineButtonType::BUTTON_PLAY_PAUSE);
    playButton->addTouchEventListener(CC_CALLBACK_2(ReplayEditorController::onPlayPauseButton, this));
    auto rwButton = m_view->getTimeLineView()->getButtons().at(TimeLineView::TimeLineButtonType::BUTTON_REWIND);
    rwButton->addTouchEventListener(CC_CALLBACK_2(ReplayEditorController::onRewindButton, this));
    auto ffButton = m_view->getTimeLineView()->getButtons().at(TimeLineView::TimeLineButtonType::BUTTON_FASTFORWARD);
    ffButton->addTouchEventListener(CC_CALLBACK_2(ReplayEditorController::onFastForwardButton, this));
    auto suButton = m_view->getTimeLineView()->getButtons().at(TimeLineView::TimeLineButtonType::BUTTON_SPEED_PLUS);
    suButton->addTouchEventListener(CC_CALLBACK_2(ReplayEditorController::onSpeedUpButton, this));
    auto sdButton = m_view->getTimeLineView()->getButtons().at(TimeLineView::TimeLineButtonType::BUTTON_SPEED_MINUS);
    sdButton->addTouchEventListener(CC_CALLBACK_2(ReplayEditorController::onSpeedDownButton, this));

    setupMouseListener(m_view->getEventDispatcher());
    
    auto director = cocos2d::Director::getInstance();
    director->setDisplayStats(false);
    
    update(0.f);
}

void ReplayEditorController::update(const float deltaTime)
{
    float time = m_view->getTimeLineView()->getCurrentTime();
    if (m_isPlaying)
    {
        time += deltaTime * m_playbackSpeed;
        m_view->getTimeLineView()->setCurrentTime(time);
    }
    updateView(time);
}

void ReplayEditorController::setupMouseListener(cocos2d::EventDispatcher* dispatcher)
{
    m_mouseListener = cocos2d::EventListenerMouse::create();
    
    m_mouseListener->onMouseMove = CC_CALLBACK_1(ReplayEditorController::onMouseMoved, this);
    m_mouseListener->onMouseDown = CC_CALLBACK_1(ReplayEditorController::onMouseDown, this);
    m_mouseListener->onMouseUp = CC_CALLBACK_1(ReplayEditorController::onMouseUp, this);

    dispatcher->addEventListenerWithFixedPriority(m_mouseListener, 1);
}

void ReplayEditorController::onMouseMoved(cocos2d::EventMouse *event)
{
    if (m_isDraggingTimeLine)
    {
        jumpToMouseCoord(event->getLocationInView());
        return;
    }
    
    const cocos2d::Vec2 aimPosition = m_gameViewController->getAimPosition(event->getLocationInView());
    const float time = m_view->getTimeLineView()->getCurrentTime();
    size_t currentFrame = m_replayModel->getSnapshotIndexForTime(time);
    const auto& snapshots = m_replayModel->getSnapshots();
    const SnapshotData& snapshot = snapshots.at(currentFrame);

    uint32_t entityID = 0;
    const bool isEntityUnderCursor = PlayerLogic::getEntityAtPoint(entityID,
                                                                   snapshot,
                                                                   aimPosition,
                                                                   0);
    if (isEntityUnderCursor)
    {
        if (!m_infoView)
        {
            m_infoView = EntityInfoView::create();
            m_infoView->setContentSize(cocos2d::Size(140.f, 140.f));
            m_infoView->setPosition(event->getLocationInView() + cocos2d::Vec2(8.f, 8.f));
            m_view->addChild(m_infoView);
        }

        const EntitySnapshot& entity = snapshot.entityData.at(entityID);
        const auto& staticData = EntityDataModel::getStaticEntityData((EntityType)entity.type);
        
        m_infoView->getNameLabel()->setString(staticData.name + "(" + std::to_string(entity.amount) + ")");
        m_infoView->getPosLabel()->setString("Pos: " + NumberFormatter::toString(entity.positionX) + ", " + NumberFormatter::toString(entity.positionY));
        m_infoView->getRotLabel()->setString("Rot: " + NumberFormatter::toString(entity.rotation));
        m_infoView->getOwnerLabel()->setString("Owner: " + std::to_string(entity.ownerID));
    }
    else if (m_infoView)
    {
        m_infoView->removeFromParentAndCleanup(true);
        m_infoView = nullptr;
    }
}

void ReplayEditorController::onMouseDown(cocos2d::EventMouse* event)
{
    if (event->getMouseButton() == cocos2d::EventMouse::MouseButton::BUTTON_LEFT)
    {
        jumpToMouseCoord(event->getLocationInView());
    }
    else if (event->getMouseButton() == cocos2d::EventMouse::MouseButton::BUTTON_RIGHT)
    {
        
    }
}

void ReplayEditorController::onMouseUp(cocos2d::EventMouse *event)
{
    if (event->getMouseButton() == cocos2d::EventMouse::MouseButton::BUTTON_LEFT)
    {
        m_isDraggingTimeLine = false;
    }
}

void ReplayEditorController::onPlayPauseButton(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type)
{
    if (type != cocos2d::ui::Widget::TouchEventType::ENDED)
    {
        return;
    }
    
    m_isPlaying = !m_isPlaying;
    updatePlayButton();
}

void ReplayEditorController::onRewindButton(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type)
{
    if (type != cocos2d::ui::Widget::TouchEventType::ENDED)
    {
        return;
    }
    
    if (m_isPlaying)
    {
        m_isPlaying = false;
        updatePlayButton();
    }
    
    const float previousTime = m_view->getTimeLineView()->getCurrentTime();
    const float time = std::max(previousTime - 1.f, 0.f);
    m_view->getTimeLineView()->setCurrentTime(time);
}

void ReplayEditorController::onFastForwardButton(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type)
{
    if (type != cocos2d::ui::Widget::TouchEventType::ENDED)
    {
        return;
    }
    
    if (m_isPlaying)
    {
        m_isPlaying = false;
        updatePlayButton();
    }
    
    const auto& snapshots = m_replayModel->getSnapshots();
    const float maxTime = snapshots.back().serverTick * m_gameModel->getFrameTime();
    const float previousTime = m_view->getTimeLineView()->getCurrentTime();
    const float time = std::min(previousTime + 1.f, maxTime);
    m_view->getTimeLineView()->setCurrentTime(time);
}

static const std::vector<float> PLAYBACK_SPEEDS = { 0.1f, 0.25f, 0.5f, 1.f, 2.f, 10.f };

void ReplayEditorController::onSpeedDownButton(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type)
{
    if (type != cocos2d::ui::Widget::TouchEventType::ENDED)
    {
        return;
    }
    
    const float playbackSpeed = m_playbackSpeed;
    for (const float speed : PLAYBACK_SPEEDS)
    {
        if (speed < playbackSpeed)
        {
            m_playbackSpeed = speed;
        }
        else
        {
            break;
        }
    }
}

void ReplayEditorController::onSpeedUpButton(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type)
{
    if (type != cocos2d::ui::Widget::TouchEventType::ENDED)
    {
        return;
    }
    
    for (const float speed : PLAYBACK_SPEEDS)
    {
        if (speed > m_playbackSpeed)
        {
            m_playbackSpeed = speed;
            break;
        }
    }
}

void ReplayEditorController::jumpToMouseCoord(const cocos2d::Vec2& coord)
{
    const cocos2d::Vec2 pointInTimeLine = m_view->getTimeLineView()->convertToNodeSpace(coord);
    const float time = m_view->getTimeLineView()->getCursorTimeAtPosition(pointInTimeLine);
    if (time != -1.f)
    {
        m_view->getTimeLineView()->setCurrentTime(time);
        m_isDraggingTimeLine = true;
        if (m_isPlaying)
        {
            m_isPlaying = false;
            updatePlayButton();
        }
    }
}

void ReplayEditorController::updatePlayButton()
{
    auto playButton = m_view->getTimeLineView()->getButtons().at(TimeLineView::TimeLineButtonType::BUTTON_PLAY_PAUSE);
    playButton->setTitleText(m_isPlaying ? "PAUSE" : "PLAY");
}

void ReplayEditorController::updateView(const float time)
{
    const auto& snapshots = m_replayModel->getSnapshots();
    if (snapshots.size() < 2)
    {
        return;
    }
    
    // Find snapshots in queue for target time
    const uint32_t targetFrame = std::floor(time * m_gameModel->getTickRate());
    size_t newFrameIndex = m_replayModel->getSnapshotIndexForFrame(targetFrame);
    newFrameIndex = std::min(snapshots.size() - 2, newFrameIndex);
    
    const bool isNewFrame = newFrameIndex != m_currentFrameIndex;
    m_currentFrameIndex = newFrameIndex;
    
    const SnapshotData& fromSnapshot = snapshots.at(m_currentFrameIndex);
    const SnapshotData& toSnapshot = snapshots.at(m_currentFrameIndex + 1);

    const float frameStartTime = fromSnapshot.serverTick * m_gameModel->getFrameTime();
    const float alphaTime = std::min(std::max((time - frameStartTime) / m_gameModel->getFrameTime(), 0.f), 1.f);
    m_gameViewController->update(m_gameModel->getFrameTime(),
                                 alphaTime,
                                 fromSnapshot,
                                 toSnapshot,
                                 isNewFrame,
                                 false);
    
    // Update timeline view
    m_view->getTimeLineView()->update(targetFrame, snapshots.size(), m_gameModel->getTickRate(), m_playbackSpeed, snapshots);
}
