#include "SettingCell.h"

#include "ButtonUtils.h"
#include "GameViewConstants.h"
#include "HUDConstants.h"
#include "HUDHelper.h"

USING_NS_CC;

SettingCell::SettingCell()
: m_name(nullptr)
, m_value(nullptr)
, m_toggleButton(nullptr)
, m_editBox(nullptr)
{
}

bool SettingCell::init()
{
    if (!cocos2d::extension::TableViewCell::init())
    {
        return false;
    }
    
    const cocos2d::Size winSize = cocos2d::Director::getInstance()->getWinSize();
    setContentSize(cocos2d::Size(winSize.width, GameViewConstants::SETTING_CELL_SIZE.height));

    auto box = HUDHelper::createItemBox(getContentSize());
    addChild(box);
    
    m_name = HUDHelper::createLabel5x7("Setting Key", GameViewConstants::FONT_SIZE_MEDIUM);
    m_name->enableOutline(cocos2d::Color4B::BLACK, 1);
    m_name->setAlignment(cocos2d::TextHAlignment::LEFT, cocos2d::TextVAlignment::CENTER);
    m_name->setAnchorPoint(cocos2d::Vec2::ANCHOR_TOP_LEFT);
    m_name->setContentSize(cocos2d::Size(winSize.width * 0.5f, 20.f));
    addChild(m_name, 1);

    m_value = HUDHelper::createLabel3x6("Setting Value", GameViewConstants::FONT_SIZE_MEDIUM);
    m_value->enableOutline(cocos2d::Color4B::BLACK, 1);
    m_value->setAlignment(cocos2d::TextHAlignment::LEFT, cocos2d::TextVAlignment::CENTER);
    m_value->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_LEFT);
    m_value->setContentSize(cocos2d::Size(winSize.width * 0.5f, 20.f));
    addChild(m_value, 1);
    
    return true;
}

void SettingCell::setup(const std::string& name,
                        const cocos2d::Value& value)
{
    m_name->setString(name);
    
    std::string description = value.getDescription();
    description.erase(std::remove(description.begin(), description.end(), '\n'), description.end());
    m_value->setString(description);
    
    removeToggleButton();
    removeEditBox();

    switch (value.getType())
    {
        case Value::Type::NONE:
            break;
        case Value::Type::BYTE:
        case Value::Type::INTEGER:
        case Value::Type::UNSIGNED:
            addEditBox(cocos2d::ui::EditBox::InputMode::NUMERIC);
            break;
        case Value::Type::FLOAT:
        case Value::Type::DOUBLE:
            addEditBox(cocos2d::ui::EditBox::InputMode::DECIMAL);
            break;
        case Value::Type::BOOLEAN:
            addToggleButton();
            break;
        case Value::Type::STRING:
            addEditBox(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
            break;
        case Value::Type::VECTOR:
            break;
        case Value::Type::MAP:
            break;
        case Value::Type::INT_KEY_MAP:
            break;
        default:
            CCASSERT(false, "Invalid type!");
            break;
    }
    
    refreshPositions();
}

void SettingCell::refreshPositions()
{
    const float PADDING = 8.f;
    const float PADDING_VERTICAL = 4.f;
    const cocos2d::Vec2 NAME_POS = cocos2d::Vec2(PADDING, getContentSize().height - PADDING_VERTICAL);
    const cocos2d::Vec2 VALUE_POS = cocos2d::Vec2(PADDING, PADDING_VERTICAL);

    m_name->setPosition(NAME_POS);
    m_value->setPosition(VALUE_POS);
    if (m_toggleButton)
    {
        const cocos2d::Vec2 BUTTON_POS = cocos2d::Vec2(getContentSize().width - (PADDING + m_toggleButton->getContentSize().width * 0.5f),
                                                       getContentSize().height * 0.5f);
        m_toggleButton->setPosition(BUTTON_POS);
    }
    if (m_editBox)
    {
        const cocos2d::Vec2 BUTTON_POS = cocos2d::Vec2(getContentSize().width - (PADDING + m_editBox->getContentSize().width * 0.5f),
                                                       getContentSize().height * 0.5f);
        m_editBox->setPosition(BUTTON_POS);
    }
}

void SettingCell::addToggleButton()
{
    if (!m_toggleButton)
    {
        m_toggleButton = ButtonUtils::createButton(getContentSize() * 0.5f,
                                                   GameViewConstants::BUTTON_SIZE,
                                                   m_value->getString());
        addChild(m_toggleButton);
    }
    else
    {
        m_toggleButton->getTitleLabel()->setString(m_value->getString());
    }
}

void SettingCell::removeToggleButton()
{
    if (m_toggleButton)
    {
        m_toggleButton->removeFromParent();
        m_toggleButton = nullptr;
    }
}

void SettingCell::addEditBox(cocos2d::ui::EditBox::InputMode mode)
{
    if (!m_editBox)
    {
        m_editBox = cocos2d::ui::EditBox::create(cocos2d::Size(getContentSize().width * 0.5f,
                                                               GameViewConstants::EDIT_BOX_SIZE_SETTING.height),
                                                 HUDConstants::HUD_SCALE9_BUTTON);
        m_editBox->setInputMode(mode);
        m_editBox->setFont(HUDConstants::FONT_5X7.c_str(),
                           GameViewConstants::FONT_SIZE_SMALL);
        m_editBox->setFontColor(cocos2d::Color3B::BLACK);
        m_editBox->setPlaceHolder(m_value->getString().c_str());
        m_editBox->setPlaceholderFont(HUDConstants::FONT_5X7.c_str(),
                                      GameViewConstants::FONT_SIZE_SMALL);
        addChild(m_editBox);
    }
    else
    {
        m_editBox->setPlaceHolder(m_value->getString().c_str());
    }
    m_editBox->setTag((int)getIdx());
}

void SettingCell::removeEditBox()
{
    if (m_editBox)
    {
        m_editBox->removeFromParent();
        m_editBox = nullptr;
    }
}
