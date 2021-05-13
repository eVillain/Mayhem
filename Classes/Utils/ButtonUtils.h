#ifndef BUTTON_UTILS_H
#define BUTTON_UTILS_H

#include "cocos2d.h"
#include <cocos/ui/UIButton.h>

class ButtonUtils
{
public:
    static cocos2d::ui::Button* createButton(const cocos2d::Vec2 position,
                                             const cocos2d::Size size,
                                             const std::string text,
                                             const float fontSize = 22.0f);
};

#endif /* BUTTON_UTILS_H */
