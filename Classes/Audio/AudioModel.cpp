#include "AudioModel.h"

AudioModel::AudioModel()
: m_masterVolume(1.0f)
, m_masterPitch(1.0f)
, m_BGMVolume(0.05f)
, m_muteAudio(false)
, m_muteBGM(false)
, m_currentBGM(-1)
{
}
