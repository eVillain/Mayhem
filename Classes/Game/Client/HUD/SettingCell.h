#ifndef SETTING_CELL_H
#define SETTING_CELL_H

#include "cocos2d.h"
#include "cocos-ext.h"

class SettingCell : public cocos2d::extension::TableViewCell
{
public:
    SettingCell();
    
    virtual bool init();
    
    void setup(const std::string& name, const cocos2d::Value& value);
    void refreshPositions();
    
    CREATE_FUNC(SettingCell);

    cocos2d::Label* getNameLabel() const { return m_name; }
    cocos2d::Label* getValueLabel() const { return m_value; }
    cocos2d::ui::Button* getToggleButton() const { return m_toggleButton; }

private:
    cocos2d::Label* m_name;
    cocos2d::Label* m_value;
    
    cocos2d::ui::Button* m_toggleButton;
    
    void addToggleButton();
};

#endif // SETTING_CELL_H
