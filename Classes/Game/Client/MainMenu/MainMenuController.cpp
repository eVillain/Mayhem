#include "MainMenuController.h"

//#include "AudioController.h"
#include "Core/Dispatcher.h"
#include "InitClientCommand.h"
#include "InitNetworkClientCommand.h"
#include "InitNetworkHostCommand.h"
#include "InitReplayEditorCommand.h"
#include "Core/Injector.h"
#include "InputController.h"
#include "Game/Client/InputModel.h"
#include "BackToMainMenuEvent.h"
#include "BackButtonPressedEvent.h"
#include "SettingsView.h"
#include "GameSettings.h"
#include "Utils/PlayerNameUtil.h"

MainMenuController::MainMenuController()
: m_view(nullptr)
, m_networkHostView(nullptr)
, m_networkClientView(nullptr)
//, m_audioController(nullptr)
{
}

MainMenuController::~MainMenuController()
{
}

bool MainMenuController::init()
{
    if (!cocos2d::Scene::init())
    {
        return false;
    }

    m_view = new MainMenuView();
    m_view->init();
    m_view->getPlayerName()->setString(getPlayerName());
    m_view->getStartGameButton()->addTouchEventListener(CC_CALLBACK_2(MainMenuController::startGameCallback, this));
    m_view->getHostGameButton()->addTouchEventListener(CC_CALLBACK_2(MainMenuController::hostGameCallback, this));
    m_view->getJoinGameButton()->addTouchEventListener(CC_CALLBACK_2(MainMenuController::joinGameCallback, this));
    m_view->getSettingsGameButton()->addTouchEventListener(CC_CALLBACK_2(MainMenuController::settingsCallback, this));
    m_view->getReplayEditorButton()->addTouchEventListener(CC_CALLBACK_2(MainMenuController::replayEditorCallback, this));
    m_view->getExitGameButton()->addTouchEventListener(CC_CALLBACK_2(MainMenuController::exitGameCallback, this));

//    Injector& injector = Injector::globalInjector();
//    m_audioController = injector.getInstance<AudioController>();
//    Dispatcher::globalDispatcher().dispatch(PlayBGMEvent(SongType::Song1));
    Dispatcher::globalDispatcher().addListener(BackToMainMenuEvent::descriptor, std::bind(&MainMenuController::onBackToMainMenu, this, std::placeholders::_1));
    Dispatcher::globalDispatcher().addListener(BackButtonPressedEvent::descriptor, std::bind(&MainMenuController::onBackButtonPressed, this, std::placeholders::_1));

    addChild(m_view);

    auto glview = cocos2d::Director::getInstance()->getOpenGLView();
    glview->setCursorVisible(true);
    glview->setViewName("Mayhem Royale - Main Menu");

//    scheduleUpdate();

    return true;
}

void MainMenuController::shutdown()
{
    removeChild(m_view);
    
    Dispatcher::globalDispatcher().removeListeners(BackToMainMenuEvent::descriptor);
    Dispatcher::globalDispatcher().removeListeners(BackButtonPressedEvent::descriptor);
}

void MainMenuController::update(float deltaTime)
{
//    m_audioController->update(deltaTime);
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

    const GameMode::Config config = {GameModeType::GAME_MODE_BATTLEROYALE, 20, 20, 1, "BitTileMap.tmx"};
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

    shutdown();

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

    shutdown();

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
    
    shutdown();

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

void MainMenuController::onBackToMainMenu(const Event& event)
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

void MainMenuController::onBackButtonPressed(const Event& event)
{
    exitGameCallback(nullptr, cocos2d::ui::Widget::TouchEventType::ENDED);
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
