#ifndef HUDHelper_h
#define HUDHelper_h

#include "cocos2d.h"
#include "cocos-ext.h"
#include "ui/CocosGUI.h"

class HUDHelper
{
public:
    static cocos2d::ui::Scale9Sprite* createHUDFill();
    static cocos2d::ui::Scale9Sprite* createHUDOutline();
    static cocos2d::ui::Scale9Sprite* createHUDRect();
    static cocos2d::ui::Scale9Sprite* createHUDArrow();

    static cocos2d::Label* createLabel3x6(const std::string& string,
                                          const size_t fontSize = 20);
    static cocos2d::Label* createLabel5x7(const std::string& string,
                                          const size_t fontSize = 20);

    static cocos2d::Node* createItemBox(const cocos2d::Size& cellSize);
    
    static cocos2d::ui::Button* createButton(const cocos2d::Vec2 position,
                                             const cocos2d::Size size,
                                             const std::string text,
                                             const float fontSize = 36.0f);
    
    static cocos2d::ui::Slider* createSlider(const float width);
    
    
    struct HUDTableView {
        cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> fill;
        cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> border;
        cocos2d::RefPtr<cocos2d::extension::TableView> tableView;
    };
    static HUDTableView createTableView(const cocos2d::Size& size,
                                        const cocos2d::Vec2& position,
                                        cocos2d::extension::TableViewDataSource* dataSource,
                                        cocos2d::extension::TableViewDelegate* delegate);

    struct HUDBox {
        cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> fill;
        cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> border;
    };
    static HUDBox createBox(const cocos2d::Size& size,
                            const cocos2d::Vec2& position);
    
    struct HUDLabelBox {
        cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> fill;
        cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> border;
        cocos2d::RefPtr<cocos2d::Label> label;
    };
    static HUDLabelBox createLabelBox(const cocos2d::Size& size,
                                      const cocos2d::Vec2& position,
                                      const std::string& string,
                                      const size_t fontSize = 20);
};

#endif /* HUDView_h */
