#include "SettingCell.h"
#include "GameViewConstants.h"
#include "HUDHelper.h"
#include "ButtonUtils.h"

USING_NS_CC;

SettingCell::SettingCell()
: m_name(nullptr)
, m_value(nullptr)
, m_toggleButton(nullptr)
{
}

bool SettingCell::init()
{
    if (!cocos2d::extension::TableViewCell::init())
    {
        return false;
    }
    
    setContentSize(GameViewConstants::SETTING_CELL_SIZE);

    auto box = HUDHelper::createItemBox(GameViewConstants::SETTING_CELL_SIZE);
    addChild(box);
    
    m_name = HUDHelper::createLabel3x6("Setting Key", GameViewConstants::FONT_SIZE_MEDIUM);
    m_name->enableOutline(cocos2d::Color4B::BLACK, 1);
    addChild(m_name, 1);

    m_value = HUDHelper::createLabel3x6("Setting Value", GameViewConstants::FONT_SIZE_MEDIUM);
    m_value->enableOutline(cocos2d::Color4B::BLACK, 1);
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
    
    switch (value.getType())
    {
        case Value::Type::NONE:
            break;
        case Value::Type::BYTE:
        case Value::Type::INTEGER:
        case Value::Type::UNSIGNED:
        case Value::Type::FLOAT:
        case Value::Type::DOUBLE:
            // Number types
            break;
        case Value::Type::BOOLEAN:
            addToggleButton();
            break;
        case Value::Type::STRING:
//            ret << v.asString() << "\n";
            break;
        case Value::Type::VECTOR:
//            ret << visitVector(v.asValueVector(), depth);
            break;
        case Value::Type::MAP:
//            ret << visitMap(v.asValueMap(), depth);
            break;
        case Value::Type::INT_KEY_MAP:
//            ret << visitMap(v.asIntKeyMap(), depth);
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
    const cocos2d::Vec2 VALUE_POS = cocos2d::Vec2(PADDING + m_value->getContentSize().width * 0.5f,
                                                  PADDING + m_value->getContentSize().height * 0.5f);
    const cocos2d::Vec2 NAME_POS = cocos2d::Vec2(PADDING + m_name->getContentSize().width * 0.5f,
                                                 VALUE_POS.y + m_name->getContentSize().height * 0.5f);

    m_value->setPosition(VALUE_POS);
    m_name->setPosition(NAME_POS);
    if (m_toggleButton)
    {
        const cocos2d::Vec2 BUTTON_POS = cocos2d::Vec2(getContentSize().width - (PADDING + m_toggleButton->getContentSize().width),
                                                       getContentSize().height * 0.5f);
        m_toggleButton->setPosition(BUTTON_POS);
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
