#ifndef AudioModel_h
#define AudioModel_h

class AudioModel
{
public:
    AudioModel();
    
    void setMasterVolume(const float volume) { m_masterVolume = volume; }
    const float getMasterVolume() const { return m_masterVolume; }
    
    void setMasterPitch(const float pitch) { m_masterPitch = pitch; }
    const float getMasterPitch() const { return m_masterPitch; }
    
    void setBGMVolume(const float volume) { m_BGMVolume = volume; }
    const float getBGMVolume() const { return m_BGMVolume; }
    
    void setMuteAudio(const bool muteAudio) { m_muteAudio = muteAudio; }
    const bool getMuteAudio() const { return m_muteAudio; }
    
    void setMuteBGM(const bool muteBGM) { m_muteBGM = muteBGM; }
    const bool getMuteBGM() const { return m_muteBGM; }
    
    void setCurrentBGM(const int newBGM) { m_currentBGM = newBGM; }
    const int getCurrentBGM() const { return m_currentBGM; }

private:
    float m_masterVolume;
    float m_masterPitch;
    float m_BGMVolume;
    
    bool m_muteAudio;
    bool m_muteBGM;
    
    int m_currentBGM;
};

#endif // AudioModel_h
