#include "AudioController.h"
#include "AudioModel.h"
#include "Game/Client/InputModel.h"
#include "Core/Injector.h"
#include "Core/Dispatcher.h"
#include "audio/include/AudioEngine.h"
#include "cocos2d.h"

#include "PlayAudioEvent.h"
#include "StopAudioEvent.h"
#include "PlayBGMEvent.h"
#include "StopBGMEvent.h"
#include "PitchAudioEvent.h"
#include "SetAudioListenerPositionEvent.h"

const std::string AudioController::SETTING_USE_POSITIONAL_AUDIO = "UsePositionalAudio";

const float PIXELS_PER_METER = 32.f;

AudioController::AudioController()
: m_model(nullptr)
, m_pitchTransition(1.0,0.0)
{
}

void AudioController::initialize()
{
    if (!Injector::globalInjector().hasMapping<AudioModel>())
    {
        Injector::globalInjector().mapSingleton<AudioModel>();
    }
    m_model = Injector::globalInjector().getInstance<AudioModel>();
    
    // We MUST preload something or the audio engine is borked
    cocos2d::experimental::AudioEngine::preload("res/audio/SFX_MenuCancel.wav");
    cocos2d::experimental::AudioEngine::preload("res/audio/SFX_MenuHover.wav");
    cocos2d::experimental::AudioEngine::preload("res/audio/SFX_MenuSelect.wav");
    
    Dispatcher::globalDispatcher().addListener(SetAudioListenerPositionEvent::descriptor, std::bind(&AudioController::onSetListenerPosition, this, std::placeholders::_1));
    Dispatcher::globalDispatcher().addListener(PlayAudioEvent::descriptor, std::bind(&AudioController::onPlayAudio, this, std::placeholders::_1));
    Dispatcher::globalDispatcher().addListener(PlayPositionalAudioEvent::descriptor, std::bind(&AudioController::onPlayPositionalAudio, this, std::placeholders::_1));
    Dispatcher::globalDispatcher().addListener(StopAudioEvent::descriptor, std::bind(&AudioController::onStopAudio, this, std::placeholders::_1));
    Dispatcher::globalDispatcher().addListener(PlayBGMEvent::descriptor, std::bind(&AudioController::onPlayBGM, this, std::placeholders::_1));
    Dispatcher::globalDispatcher().addListener(StopBGMEvent::descriptor, std::bind(&AudioController::onStopBGM, this, std::placeholders::_1));
    Dispatcher::globalDispatcher().addListener(PitchAudioEvent::descriptor, std::bind(&AudioController::onPitchAudio, this, std::placeholders::_1));
}

void AudioController::setListenerPosition(const cocos2d::Vec2& position)
{
    cocos2d::experimental::AudioEngine::setListenerPosition(position / PIXELS_PER_METER);
}

void AudioController::update(const float deltaTime)
{
    float currentPitch = m_model->getMasterPitch();
    float targetPitch = m_pitchTransition.targetPitch;
    float rateOfChange = m_pitchTransition.rateOfChange;
    if (currentPitch != targetPitch && rateOfChange != 0.0)
    {
        currentPitch += rateOfChange * deltaTime;
        if ((currentPitch < targetPitch && rateOfChange < 0.0) ||
            (currentPitch > targetPitch && rateOfChange > 0.0))
        {
            currentPitch = targetPitch;
            m_pitchTransition.rateOfChange = 0.0;
        }
        setPitch(currentPitch);
    }
}

int AudioController::playAudio(const std::string& audioFile, const float volume) const
{    
    int audioInstance = cocos2d::experimental::AudioEngine::play2d("res/audio/" + audioFile, false, volume * m_model->getMasterVolume());
//    cocos2d::experimental::AudioEngine::setPitch(audioInstance, m_model->getMasterPitch());
    return audioInstance;
}

int AudioController::playPositionalAudio(const std::string& audioFile,
                                         const cocos2d::Vec2& position,
                                         const float volume) const
{
    int audioInstance = cocos2d::experimental::AudioEngine::play2d("res/audio/" + audioFile, false, volume * m_model->getMasterVolume());
    if (audioInstance > 0)
    {
        cocos2d::experimental::AudioEngine::setAudioPosition(audioInstance, position / PIXELS_PER_METER);
    }
    
    return audioInstance;
}

void AudioController::stopAudio(const int audio) const
{
    cocos2d::experimental::AudioEngine::stop(audio);
}

void AudioController::setPitch(const float pitch)
{
    m_model->setMasterPitch(pitch);
    if (m_model->getCurrentBGM() >= 0)
    {
//        cocos2d::experimental::AudioEngine::setPitch(m_model->getCurrentBGM(), pitch);
    }
}

void AudioController::onSetListenerPosition(const Event &event)
{
    const SetAudioListenerPositionEvent& audioEvent = static_cast<const SetAudioListenerPositionEvent&>(event);
    setListenerPosition(audioEvent.getPosition());
}

void AudioController::onPlayAudio(const Event &event)
{
    const PlayAudioEvent& audioEvent = static_cast<const PlayAudioEvent&>(event);
    playAudio(audioEvent.getAudioFile(), audioEvent.getVolume());
}

void AudioController::onPlayPositionalAudio(const Event &event)
{
    const PlayPositionalAudioEvent& audioEvent = static_cast<const PlayPositionalAudioEvent&>(event);
    playPositionalAudio(audioEvent.getAudioFile(), audioEvent.getPosition(), audioEvent.getVolume());
}

void AudioController::onStopAudio(const Event &event)
{
    const StopAudioEvent& audioEvent = static_cast<const StopAudioEvent&>(event);
    const int audioInstance = audioEvent.getAudio();
    stopAudio(audioInstance);
}

void AudioController::onPlayBGM(const Event& event)
{
//    float volume = m_model->getMuteBGM() ? 0.0f : m_model->getMasterVolume()*m_model->getBGMVolume();
//    const int newBGM = cocos2d::experimental::AudioEngine::play2d(BG_SONG_1.c_str(), true, volume);
//    cocos2d::experimental::AudioEngine::setPitch(newBGM, m_model->getMasterPitch());
//    m_model->setCurrentBGM(newBGM);
}

void AudioController::onStopBGM(const Event& event)
{
    cocos2d::experimental::AudioEngine::stop(m_model->getCurrentBGM());
    m_model->setCurrentBGM(-1);
}

void AudioController::onPitchAudio(const Event& event)
{
    const PitchAudioEvent& audioEvent = static_cast<const PitchAudioEvent&>(event);
    const float duration = audioEvent.getDuration();
    const float pitch = audioEvent.getNewPitch();
    if (duration == 0.0)
    {
        setPitch(pitch);
        m_pitchTransition.rateOfChange = 0.0;
    }
    else
    {
        m_pitchTransition.targetPitch = pitch;
        m_pitchTransition.rateOfChange = (pitch - m_model->getMasterPitch()) / duration;
    }
}

