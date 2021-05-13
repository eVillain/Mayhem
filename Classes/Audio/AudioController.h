#ifndef AudioController_h
#define AudioController_h

#include "cocos2d.h"
#include <string>

class AudioModel;
class Event;

class AudioController
{
public:
    static const std::string SETTING_USE_POSITIONAL_AUDIO;

    AudioController();
    
    void initialize();
    void setListenerPosition(const cocos2d::Vec2& position);
    
    void update(const float deltaTime);
    
    int playAudio(const std::string& audioFile,
                  const float volume = 1.0f) const;
    int playPositionalAudio(const std::string& audioFile,
                            const cocos2d::Vec2& position,
                            const float volume = 1.0f) const;
    void stopAudio(const int audio) const;

    void setPitch(const float pitch);
    
private:
    struct AudioPitchTransition
    {
        float targetPitch;
        float rateOfChange;
        AudioPitchTransition(float target, float rate) :
        targetPitch(target), rateOfChange(rate){};
    };
    
    std::shared_ptr<AudioModel> m_model;
    AudioPitchTransition m_pitchTransition;
    
    void onSetListenerPosition(const Event& event);
    void onPlayAudio(const Event& event);
    void onPlayPositionalAudio(const Event& event);
    void onStopAudio(const Event& event);
    void onPlayBGM(const Event& event);
    void onStopBGM(const Event& event);
    void onPitchAudio(const Event& event);
};

#endif /* AudioController_h */
