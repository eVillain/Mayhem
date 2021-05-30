#ifndef AudioController_h
#define AudioController_h

#include "cocos2d.h"
#include <string>

class AudioModel;
class GameSettings;

class AudioController
{
public:
    static const std::string SETTING_USE_POSITIONAL_AUDIO;

    AudioController(std::shared_ptr<AudioModel> model,
                    std::shared_ptr<GameSettings> settings);
    ~AudioController();
    
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
    std::shared_ptr<GameSettings> m_gameSettings;
    AudioPitchTransition m_pitchTransition;
};

#endif /* AudioController_h */
