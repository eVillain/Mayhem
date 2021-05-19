#include "AudioController.h"

#include "AudioModel.h"
#include "Game/Client/InputModel.h"
#include "audio/include/AudioEngine.h"
#include "cocos2d.h"
#include "GameSettings.h"

const std::string AudioController::SETTING_USE_POSITIONAL_AUDIO = "UsePositionalAudio";

static const float PIXELS_PER_METER = 64.f;

AudioController::AudioController(std::shared_ptr<AudioModel> model,
                                 std::shared_ptr<GameSettings> settings)
: m_model(model)
, m_gameSettings(settings)
, m_pitchTransition(1.0,0.0)
{
    printf("AudioController:: constructor: %p\n", this);
}

AudioController::~AudioController()
{
    printf("AudioController:: destructor: %p\n", this);
}

void AudioController::initialize()
{
    // We MUST preload something or the audio engine is borked
    cocos2d::experimental::AudioEngine::preload("res/audio/SFX_MenuCancel.wav");
    cocos2d::experimental::AudioEngine::preload("res/audio/SFX_MenuHover.wav");
    cocos2d::experimental::AudioEngine::preload("res/audio/SFX_MenuSelect.wav");
}

void AudioController::setListenerPosition(const cocos2d::Vec2& position)
{
    const cocos2d::Value& usePositionalAudioSetting = m_gameSettings->getValue(SETTING_USE_POSITIONAL_AUDIO, cocos2d::Value(false));
    if (!usePositionalAudioSetting.asBool())
    {
        return;
    }
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
    const cocos2d::Value& usePositionalAudioSetting = m_gameSettings->getValue(SETTING_USE_POSITIONAL_AUDIO, cocos2d::Value(false));
    if (!usePositionalAudioSetting.asBool())
    {
        return playAudio(audioFile, volume);
    }

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
//    if (m_model->getCurrentBGM() >= 0)
//    {
//        cocos2d::experimental::AudioEngine::setPitch(m_model->getCurrentBGM(), pitch);
//    }
}

//void AudioController::onPlayBGM(const Event& event)
//{
//    float volume = m_model->getMuteBGM() ? 0.0f : m_model->getMasterVolume()*m_model->getBGMVolume();
//    const int newBGM = cocos2d::experimental::AudioEngine::play2d(BG_SONG_1.c_str(), true, volume);
//    cocos2d::experimental::AudioEngine::setPitch(newBGM, m_model->getMasterPitch());
//    m_model->setCurrentBGM(newBGM);
//}

//void AudioController::onStopBGM(const Event& event)
//{
//    cocos2d::experimental::AudioEngine::stop(m_model->getCurrentBGM());
//    m_model->setCurrentBGM(-1);
//}

//void AudioController::onPitchAudio(const Event& event)
//{
//    const PitchAudioEvent& audioEvent = static_cast<const PitchAudioEvent&>(event);
//    const float duration = audioEvent.getDuration();
//    const float pitch = audioEvent.getNewPitch();
//    if (duration == 0.0)
//    {
//        setPitch(pitch);
//        m_pitchTransition.rateOfChange = 0.0;
//    }
//    else
//    {
//        m_pitchTransition.targetPitch = pitch;
//        m_pitchTransition.rateOfChange = (pitch - m_model->getMasterPitch()) / duration;
//    }
//}

