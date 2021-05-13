#ifndef GameSettings_h
#define GameSettings_h

#include "cocos2d.h"

class GameSettings
{
public:
    static const std::string DEFAULT_SETTINGS_FILE;
    static const std::string SETTING_SAVE_SETTINGS_ON_EXIT;

    GameSettings();
    ~GameSettings();

    void load(const std::string& plist);
    void save(const std::string& plist);
    
    void addValueChangeCallback(const std::string& key,
                                std::function<void(const std::string&, cocos2d::Value&)> callback);
    void removeValueChangeCallback(const std::string& key);
    
    const cocos2d::ValueMap& getValues() const { return m_values; }

    cocos2d::Value& getValue(const std::string& key, cocos2d::Value defaultValue);
    void setValue(const std::string& key, cocos2d::Value value);


private:
    cocos2d::ValueMap m_values;
    std::map<std::string, std::function<void(const std::string&, cocos2d::Value&)>> m_callbacks;
};

#endif /* GameSettings_h */
