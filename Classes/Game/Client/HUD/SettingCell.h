#ifndef SETTING_CELL_H
#define SETTING_CELL_H

#include "cocos2d.h"
#include "cocos-ext.h"
#include <cocos/ui/UIEditBox/UIEditBox.h>

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
    cocos2d::ui::EditBox* getEditBox() const { return m_editBox; }

private:
    cocos2d::Label* m_name;
    cocos2d::Label* m_value;
    
    cocos2d::ui::Button* m_toggleButton;
    cocos2d::ui::EditBox* m_editBox;

    void addToggleButton();
    void removeToggleButton();
    
    void addEditBox(cocos2d::ui::EditBox::InputMode mode);
    void removeEditBox();
};

#endif // SETTING_CELL_H
