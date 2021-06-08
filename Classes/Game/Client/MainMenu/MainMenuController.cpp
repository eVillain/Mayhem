#include "MainMenuController.h"

#include "Core/Dispatcher.h"
#include "InitClientCommand.h"
#include "InitNetworkClientCommand.h"
#include "InitNetworkHostCommand.h"
#include "InitReplayEditorCommand.h"
#include "Core/Injector.h"
#include "Game/Client/InputModel.h"
#include "BackToMainMenuEvent.h"
#include "InputActionEvent.h"
#include "InputConstants.h"
#include "SettingsView.h"
#include "GameSettings.h"
#include "Utils/PlayerNameUtil.h"
#include "EntityView.h"
#include "ShutdownMainMenuCommand.h"

MainMenuController::MainMenuController()
: m_view(nullptr)
, m_networkHostView(nullptr)
, m_networkClientView(nullptr)
{
    printf("MainMenuController:: constructor: %p\n", this);
}

MainMenuController::~MainMenuController()
{
    printf("MainMenuController:: destructor: %p\n", this);
}

bool MainMenuController::init()
{
    if (!cocos2d::Scene::init())
    {
        return false;
    }

    m_view = MainMenuView::create();
    m_view->getPlayerName()->setString(getPlayerName());
    m_view->getStartGameButton()->addTouchEventListener(CC_CALLBACK_2(MainMenuController::startGameCallback, this));
    m_view->getHostGameButton()->addTouchEventListener(CC_CALLBACK_2(MainMenuController::hostGameCallback, this));
    m_view->getJoinGameButton()->addTouchEventListener(CC_CALLBACK_2(MainMenuController::joinGameCallback, this));
    m_view->getSettingsGameButton()->addTouchEventListener(CC_CALLBACK_2(MainMenuController::settingsCallback, this));
    m_view->getReplayEditorButton()->addTouchEventListener(CC_CALLBACK_2(MainMenuController::replayEditorCallback, this));
    m_view->getExitGameButton()->addTouchEventListener(CC_CALLBACK_2(MainMenuController::exitGameCallback, this));

    Dispatcher::globalDispatcher().addListener<BackToMainMenuEvent>(std::bind(&MainMenuController::onBackToMainMenu,
                                                                              this, std::placeholders::_1),
                                                                    this);
    Dispatcher::globalDispatcher().addListener<InputActionEvent>(std::bind(&MainMenuController::onInputAction,
                                                                                 this, std::placeholders::_1),
                                                                       this);

    addChild(m_view);

    auto glview = cocos2d::Director::getInstance()->getOpenGLView();
    glview->setCursorVisible(true);
    glview->setViewName("Mayhem Royale - Main Menu");

    scheduleUpdate();

    return true;
}

void MainMenuController::shutdown()
{
    removeChild(m_view);
    m_view = nullptr;
    
    unscheduleUpdate();

    Dispatcher::globalDispatcher().removeListener<BackToMainMenuEvent>(this);
    Dispatcher::globalDispatcher().removeListener<InputActionEvent>(this);
    
    ShutdownMainMenuCommand shutdownMainMenu;
    shutdownMainMenu.run();
}

void MainMenuController::update(float deltaTime)
{
    Injector& injector = Injector::globalInjector();
    auto inputModel = injector.getInstance<InputModel>();
    
    cocos2d::Vec2 playerToMouse;
    if (inputModel->getActiveGamepad() == -1)
    {
        const cocos2d::Vec2 aimPoint = inputModel->getMouseCoord();
        playerToMouse = (aimPoint - m_view->getPlayerNode()->getPosition()).getNormalized();
    }
    else
    {
            const cocos2d::Vec2 aimDirection = cocos2d::Vec2(inputModel->getInputValue(InputConstants::ACTION_AIM_RIGHT),
                                                             -inputModel->getInputValue(InputConstants::ACTION_AIM_UP));
            playerToMouse = aimDirection.getNormalized();
    }
    auto playerView = m_view->getPlayerView();
    auto leftArm = playerView->getSecondarySprites().at(EntityView::ARM_LEFT_INDEX);
    leftArm->setRotation(playerToMouse.getAngle() * (-180.f / M_PI) - 90.f);
}

const std::string MainMenuController::getPlayerName() const
{
    auto settings = Injector::globalInjector().getInstance<GameSettings>();
    const cocos2d::Value& playerNameSetting = settings->getValue(GameSettings::SETTING_PLAYER_NAME, cocos2d::Value(""));
    if (!playerNameSetting.asString().empty())
    {
        return playerNameSetting.asString();
    }

    // Save default name so it can be edited in the settings menu
    const std::string playerName = PlayerNameUtil::getLoginUserName();
    settings->setValue(GameSettings::SETTING_PLAYER_NAME, cocos2d::Value(playerName));
    return playerName;
}

void MainMenuController::startGameCallback(cocos2d::Ref* ref,
                                           cocos2d::ui::Widget::TouchEventType type)
{
    if (type != cocos2d::ui::Widget::TouchEventType::ENDED)
    {
        return;
    }

    shutdown();

    const GameMode::Config config = {GameModeType::GAME_MODE_DEATHMATCH, 20, 20, 1, "BitTileMap.tmx"};
    InitClientCommand initGame(InitClientCommand::Mode::FAKE_CLIENT, config);
    initGame.run();
}

void MainMenuController::hostGameCallback(cocos2d::Ref* ref,
                                          cocos2d::ui::Widget::TouchEventType type)
{
    if (type != cocos2d::ui::Widget::TouchEventType::ENDED)
    {
        return;
    }

    removeChild(m_view);

    m_networkHostView = new NetworkHostView();
    m_networkHostView->init();
    addChild(m_networkHostView);

    InitNetworkHostCommand initNetwork(m_networkHostView);
    initNetwork.run();
}

void MainMenuController::joinGameCallback(cocos2d::Ref* ref,
                                          cocos2d::ui::Widget::TouchEventType type)
{
    if (type != cocos2d::ui::Widget::TouchEventType::ENDED)
    {
        return;
    }

    removeChild(m_view);

    m_networkClientView = new NetworkClientView();
    m_networkClientView->init();
    addChild(m_networkClientView);

    InitNetworkClientCommand initNetwork(m_networkClientView);
    initNetwork.run();
}

void MainMenuController::settingsCallback(cocos2d::Ref* ref,
                                          cocos2d::ui::Widget::TouchEventType type)
{
    if (type != cocos2d::ui::Widget::TouchEventType::ENDED)
    {
        return;
    }
    
    removeChild(m_view);

    auto settingsView = new SettingsView(Injector::globalInjector().getInstance<GameSettings>());
    settingsView->initialize();
    addChild(settingsView->getLayer());

    settingsView->getExitButton()->addTouchEventListener(std::bind([this](){
        this->onBackToMainMenu(BackToMainMenuEvent());
    }));
}

void MainMenuController::replayEditorCallback(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType type)
{
    if (type != cocos2d::ui::Widget::TouchEventType::ENDED)
    {
        return;
    }

    shutdown();

    InitReplayEditorCommand initReplay;
    initReplay.run();
}

void MainMenuController::exitGameCallback(cocos2d::Ref* ref,
                                          cocos2d::ui::Widget::TouchEventType type)
{
    if (type != cocos2d::ui::Widget::TouchEventType::ENDED)
    {
        return;
    }

    shutdown();

    exitGame();
}

void MainMenuController::onBackToMainMenu(const BackToMainMenuEvent& event)
{
    removeAllChildren();
    
    addChild(m_view);

    if (m_networkHostView)
    {
        m_networkHostView = nullptr;
    }
    
    if (m_networkClientView)
    {
        m_networkClientView = nullptr;
    }
}

void MainMenuController::onInputAction(const InputActionEvent& event)
{
    if (event.previousValue == 1.f || event.value < 1.f)
    {
        return;
    }
    if (event.action == InputConstants::ACTION_BACK)
    {
        exitGameCallback(nullptr, cocos2d::ui::Widget::TouchEventType::ENDED);
    }
}

void MainMenuController::exitGame()
{
    Injector::globalInjector().getInstance<GameSettings>()->save(GameSettings::DEFAULT_SETTINGS_FILE);
    //Close the cocos2d-x game scene and quit the application
    cocos2d::Director::getInstance()->end();
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
