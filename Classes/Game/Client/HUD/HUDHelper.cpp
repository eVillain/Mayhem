#include "HUDHelper.h"
#include "HUDConstants.h"

cocos2d::ui::Scale9Sprite* HUDHelper::createHUDFill()
{
    return cocos2d::ui::Scale9Sprite::create(HUDConstants::PIXEL_FILL);
}

cocos2d::ui::Scale9Sprite* HUDHelper::createHUDOutline()
{
    return cocos2d::ui::Scale9Sprite::create(HUDConstants::PIXEL_OUTLINE);
}

cocos2d::ui::Scale9Sprite* HUDHelper::createHUDRect()
{
    return cocos2d::ui::Scale9Sprite::create(HUDConstants::PIXEL_RECT);
}

cocos2d::ui::Scale9Sprite* HUDHelper::createHUDArrow()
{
    return cocos2d::ui::Scale9Sprite::create(HUDConstants::HUD_SCALE9_ARROW);
}

cocos2d::Label* HUDHelper::createLabel3x6(const std::string& string,
                                          const size_t fontSize /* = 20 */)
{
    return cocos2d::Label::createWithTTF(string, HUDConstants::FONT_3X6, fontSize);

}

cocos2d::Label* HUDHelper::createLabel5x7(const std::string& string,
                                          const size_t fontSize /* = 20 */)
{
    return cocos2d::Label::createWithTTF(string, HUDConstants::FONT_5X7, fontSize);
}

cocos2d::Node* HUDHelper::createItemBox(const cocos2d::Size& cellSize)
{
    cocos2d::Node* itemNode = cocos2d::Node::create();
    
    cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> cellBG = HUDHelper::createHUDRect();
    cellBG->setColor(HUDConstants::TABLEVIEW_CELL_COLOR);
    cellBG->setOpacity(63);
    cellBG->setPreferredSize(cellSize - cocos2d::Size(2,2));
    cellBG->setPosition(cellSize * 0.5f);
    itemNode->addChild(cellBG, -1);
    
    cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> cellBorder = HUDHelper::createHUDOutline();
    cellBorder->setColor(cocos2d::Color3B::GRAY);
    cellBorder->setPreferredSize(cellSize - cocos2d::Size(4,4));
    cellBorder->setPosition(cellSize * 0.5f);
    itemNode->addChild(cellBorder);
    
    return itemNode;
}

cocos2d::ui::Button* HUDHelper::createButton(const cocos2d::Vec2 position,
                                             const cocos2d::Size size,
                                             const std::string text,
                                             const float fontSize /*= 22.0f*/)
{
    cocos2d::ui::Button* button = cocos2d::ui::Button::create(HUDConstants::HUD_SCALE9_BUTTON,
                                                              HUDConstants::HUD_SCALE9_BUTTON_PRESSED,
                                                              HUDConstants::HUD_SCALE9_BUTTON_INACTIVE);
    button->setPosition(position);
    button->setContentSize(size);
    button->setScale9Enabled(true);
    button->setColor(HUDConstants::BUTTON_COLOR);
    
    button->setTitleText(text);
    button->setTitleFontName(HUDConstants::FONT_5X7);
    button->setTitleFontSize(fontSize);
    button->setTitleColor(cocos2d::Color3B::BLACK);
    
    cocos2d::ui::Scale9Sprite* border = createHUDOutline();
    border->setContentSize(size - cocos2d::Size(4,4));
    border->setPosition(size * 0.5f);
    border->setColor(HUDConstants::BUTTON_BORDER_COLOR);
    button->addChild(border);
    
    return button;
}

cocos2d::ui::Slider* HUDHelper::createSlider(const float width)
{
    auto slider = cocos2d::ui::Slider::create();
    slider->loadBarTexture(HUDConstants::HUD_SCALE9_SLIDER_BAR);
    slider->loadProgressBarTexture(HUDConstants::HUD_SCALE9_SLIDER_PROGRESSBAR);
    slider->loadSlidBallTextureNormal(HUDConstants::HUD_SCALE9_SLIDER_BALL);
    slider->setScale9Enabled(true);
    slider->setContentSize(cocos2d::Size(width, 24.f));
    return slider;
}

HUDHelper::HUDTableView HUDHelper::createTableView(const cocos2d::Size& size,
                                                   const cocos2d::Vec2& position,
                                                   cocos2d::extension::TableViewDataSource* dataSource,
                                                   cocos2d::extension::TableViewDelegate* delegate)
{
    const cocos2d::Size TABLE_SIZE = size - cocos2d::Size(4, 4);
    const cocos2d::Size BORDER_SIZE = size - cocos2d::Size(2, 2);

    HUDHelper::HUDTableView tableView;
    
    tableView.fill = HUDHelper::createHUDFill();
    tableView.fill->setPreferredSize(size);
    tableView.fill->setAnchorPoint(cocos2d::Vec2::ZERO);
    tableView.fill->setColor(HUDConstants::HUD_FILL_COLOR);
    tableView.border = HUDHelper::createHUDOutline();
    tableView.border->setPreferredSize(BORDER_SIZE);
    tableView.border->setAnchorPoint(cocos2d::Vec2::ZERO);
    tableView.border->setColor(HUDConstants::HUD_BORDER_COLOR);
    tableView.tableView = cocos2d::extension::TableView::create(dataSource, TABLE_SIZE);
    tableView.tableView->setDelegate(delegate);
    
    tableView.fill->setPosition(position);
    tableView.border->setPosition(position + cocos2d::Vec2::ONE);
    tableView.tableView->setPosition(position + cocos2d::Vec2(2,2));
    
    return tableView;
}

HUDHelper::HUDBox HUDHelper::createBox(const cocos2d::Size& size,
                                       const cocos2d::Vec2& position)
{
    const cocos2d::Size BORDER_SIZE = size - cocos2d::Size(2, 2);
    HUDHelper::HUDBox box;
    
    box.fill = HUDHelper::createHUDFill();
    box.fill->setPreferredSize(size);
    box.fill->setAnchorPoint(cocos2d::Vec2::ZERO);
    box.fill->setColor(HUDConstants::HUD_FILL_COLOR);
    box.border = HUDHelper::createHUDOutline();
    box.border->setPreferredSize(BORDER_SIZE);
    box.border->setAnchorPoint(cocos2d::Vec2::ZERO);
    box.border->setColor(HUDConstants::HUD_BORDER_COLOR);
    box.fill->setPosition(position);
    box.border->setPosition(position + cocos2d::Vec2::ONE);
    
    return box;
}

HUDHelper::HUDLabelBox HUDHelper::createLabelBox(const cocos2d::Size& size,
                                                  const cocos2d::Vec2& position,
                                                  const std::string& string,
                                                  const size_t fontSize /* = 20 */)
{
    const cocos2d::Size LABEL_SIZE = size - cocos2d::Size(4, 4);
    const cocos2d::Size BORDER_SIZE = size - cocos2d::Size(2, 2);

    HUDHelper::HUDLabelBox labelBox;
    
    labelBox.fill = HUDHelper::createHUDFill();
    labelBox.fill->setPreferredSize(size);
    labelBox.fill->setAnchorPoint(cocos2d::Vec2::ZERO);
    labelBox.fill->setColor(HUDConstants::HUD_FILL_COLOR);
    labelBox.border = HUDHelper::createHUDOutline();
    labelBox.border->setPreferredSize(BORDER_SIZE);
    labelBox.border->setAnchorPoint(cocos2d::Vec2::ZERO);
    labelBox.border->setColor(HUDConstants::HUD_BORDER_COLOR);
    labelBox.label = cocos2d::Label::createWithTTF(string,
                                                   HUDConstants::FONT_5X7,
                                                   fontSize);
    labelBox.label->setAnchorPoint(cocos2d::Vec2::ZERO);
    labelBox.label->setDimensions(LABEL_SIZE.width, LABEL_SIZE.height);
    labelBox.label->enableOutline(cocos2d::Color4B::BLACK, 1);
    labelBox.label->setAlignment(cocos2d::TextHAlignment::CENTER,
                                 cocos2d::TextVAlignment::CENTER);

    labelBox.fill->setPosition(position);
    labelBox.border->setPosition(position + cocos2d::Vec2::ONE);
    labelBox.label->setPosition(position + cocos2d::Vec2(2,2));
    
    return labelBox;
}
