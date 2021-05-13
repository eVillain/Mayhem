#include "GameSettings.h"

const std::string GameSettings::DEFAULT_SETTINGS_FILE = "res/Settings.plist";
const std::string GameSettings::SETTING_SAVE_SETTINGS_ON_EXIT = "SaveSettingsOnExit";

GameSettings::GameSettings()
{
}

GameSettings::~GameSettings()
{
    const cocos2d::Value& saveOnExitSetting = getValue(SETTING_SAVE_SETTINGS_ON_EXIT, cocos2d::Value(true));
    if (saveOnExitSetting.asBool())
    {
        save(DEFAULT_SETTINGS_FILE);
    }
}

void GameSettings::load(const std::string& plist)
{
    const std::string fullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename(plist);
    m_values = cocos2d::FileUtils::getInstance()->getValueMapFromFile(fullPath);

    printf("GameSettings::load loaded %zu values from %s\n", m_values.size(), fullPath.c_str());
}

void GameSettings::save(const std::string& plist)
{
    const std::string fullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename(plist);
    cocos2d::FileUtils::getInstance()->writeToFile(m_values, fullPath);
}

void GameSettings::addValueChangeCallback(const std::string& key,
                                          std::function<void(const std::string&, cocos2d::Value&)> callback)
{
    const auto& it = m_callbacks.find(key);
    if (it != m_callbacks.end())
    {
        printf("GameSettings::load warning, overriding callback for %s\n", it->first.c_str());
    }
    m_callbacks[key] = callback;
}

void GameSettings::removeValueChangeCallback(const std::string& key)
{
    const auto& it = m_callbacks.find(key);
    if (it != m_callbacks.end())
    {
        printf("GameSettings::load warning, no callback to remove for %s\n", it->first.c_str());
        return;
    }
    
    m_callbacks.erase(it);
}

cocos2d::Value& GameSettings::getValue(const std::string& key, cocos2d::Value defaultValue)
{
    const auto& it = m_values.find(key);
    if (it == m_values.end())
    {
        m_values[key] = defaultValue;
        return m_values[key];
    }
    
    return it->second;
}

void GameSettings::setValue(const std::string& key, cocos2d::Value value)
{
    const auto& it = m_values.find(key);
    if (it == m_values.end())
    {
        return;
    }
    
    it->second = value;
    
    const auto& cbIt = m_callbacks.find(key);
    if (cbIt == m_callbacks.end())
    {
        return;
    }
    
    cbIt->second(key, value);
}
