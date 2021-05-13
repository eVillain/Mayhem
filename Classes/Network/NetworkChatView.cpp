#include "NetworkChatView.h"
#include "GameViewConstants.h"
#include "HUDHelper.h"
#include "ButtonUtils.h"

USING_NS_CC;

NetworkChatView::NetworkChatView()
: m_sendMessageButton(nullptr)
, m_messageEditBox(nullptr)
, m_messagesTableView(nullptr)
, m_messagesHeaderBG(nullptr)
, m_messagesHeaderBorder(nullptr)
, m_messagesHeaderLabel(nullptr)
, m_messagesBG(nullptr)
, m_messagesBorder(nullptr)
{
}

bool NetworkChatView::init()
{
    if (!cocos2d::Node::init())
    {
        return false;
    }
    
    const cocos2d::Size winSize = cocos2d::Director::getInstance()->getWinSize();
    
    const float LEFT_PADDING = GameViewConstants::GAMEMODE_CELL_SIZE.width;
    const cocos2d::Vec2 MESSAGE_EDIT_BOX_POSITION = cocos2d::Vec2(LEFT_PADDING, 0.f) + GameViewConstants::EDIT_BOX_SIZE_MESSAGE * 0.5f;
    const cocos2d::Size MESSAGE_BOX_SIZE = cocos2d::Size(GameViewConstants::EDITBOX_DEFAULT_WIDTH, 200.f);
    const cocos2d::Vec2 MESSAGE_BOX_POSITION = cocos2d::Vec2(GameViewConstants::EDITBOX_DEFAULT_WIDTH,
                                                             GameViewConstants::EDIT_BOX_SIZE_MESSAGE.height + MESSAGE_BOX_SIZE.height * 0.5f);
    const cocos2d::Vec2 MESSAGE_BOX_HEADER_POSITION = MESSAGE_BOX_POSITION + cocos2d::Size(0.f,
                                                                                           MESSAGE_BOX_SIZE.height * 0.5f +  GameViewConstants::EDIT_BOX_HEADER_SIZE_MESSAGE.height * 0.5f);
    m_messageEditBox = cocos2d::ui::EditBox::create(GameViewConstants::EDIT_BOX_SIZE_MESSAGE,
                                                    GameViewConstants::HUD_SCALE9_BUTTON);
    m_messageEditBox->setInputMode(cocos2d::ui::EditBox::InputMode::SINGLE_LINE);
    m_messageEditBox->setFont(GameViewConstants::FONT_5X7.c_str(),
                              GameViewConstants::FONT_SIZE_SMALL);
    m_messageEditBox->setFontColor(cocos2d::Color3B::BLACK);
    m_messageEditBox->setPlaceHolder("Enter message");
    m_messageEditBox->setPlaceholderFont(GameViewConstants::FONT_5X7.c_str(), GameViewConstants::FONT_SIZE_SMALL);
    m_messageEditBox->setPosition(MESSAGE_EDIT_BOX_POSITION);
    addChild(m_messageEditBox);
    
    m_messagesTableView = cocos2d::extension::TableView::create(this, GameViewConstants::CHAT_BOX_SIZE_MESSAGE);
    m_messagesTableView->setDelegate(this);
    m_messagesTableView->setPosition(MESSAGE_BOX_POSITION - (MESSAGE_BOX_SIZE * 0.5));
    addChild(m_messagesTableView);
    
    m_messagesHeaderBG = HUDHelper::createHUDFill();
    m_messagesHeaderBG->setPreferredSize(GameViewConstants::EDIT_BOX_HEADER_SIZE_MESSAGE);
    m_messagesHeaderBG->setPosition(MESSAGE_BOX_HEADER_POSITION);
    m_messagesHeaderBG->setColor(GameViewConstants::TABLEVIEW_HEADER_BG_COLOR);
    m_messagesHeaderBG->setOpacity(63);
    addChild(m_messagesHeaderBG);
    m_messagesHeaderBorder = HUDHelper::createHUDOutline();
    m_messagesHeaderBorder->setPreferredSize(GameViewConstants::EDIT_BOX_HEADER_SIZE_MESSAGE - cocos2d::Size(2,2));
    m_messagesHeaderBorder->setPosition(MESSAGE_BOX_HEADER_POSITION);
    m_messagesHeaderBorder->setColor(GameViewConstants::BUTTON_BORDER_COLOR);
    addChild(m_messagesHeaderBorder);
    m_messagesHeaderLabel = HUDHelper::createLabel3x6("Chat", GameViewConstants::FONT_SIZE_LARGE);
    m_messagesHeaderLabel->setPosition(MESSAGE_BOX_HEADER_POSITION);
    addChild(m_messagesHeaderLabel);
    
    m_messagesBG = HUDHelper::createHUDFill();
    m_messagesBG->setPreferredSize(MESSAGE_BOX_SIZE);
    m_messagesBG->setPosition(MESSAGE_BOX_POSITION);
    m_messagesBG->setColor(GameViewConstants::TABLEVIEW_BG_COLOR);
    m_messagesBG->setOpacity(63);
    addChild(m_messagesBG);
    m_messagesBorder = HUDHelper::createHUDOutline();
    m_messagesBG->setPreferredSize(MESSAGE_BOX_SIZE - cocos2d::Size(2,2));
    m_messagesBG->setPosition(MESSAGE_BOX_POSITION);
    m_messagesBorder->setColor(GameViewConstants::BUTTON_BORDER_COLOR);
    addChild(m_messagesBorder);
    
    const cocos2d::Vec2 SEND_BUTTON_POSITION = cocos2d::Vec2(LEFT_PADDING + GameViewConstants::EDIT_BOX_SIZE_MESSAGE.width + GameViewConstants::BUTTON_SIZE_SEND.width * 0.5f,
                                                             MESSAGE_EDIT_BOX_POSITION.y);
    m_sendMessageButton = ButtonUtils::createButton(SEND_BUTTON_POSITION,
                                                    GameViewConstants::BUTTON_SIZE_SEND,
                                                    "Send",
                                                    GameViewConstants::FONT_SIZE_MEDIUM);
    addChild(m_sendMessageButton);
    
    return true;
}

void NetworkChatView::addMessage(const std::string& message, const int senderID)
{
    const std::string formattedMsg = std::to_string(senderID) + ": " + message;
    m_messages.push_back(formattedMsg);
    m_messagesTableView->reloadData();
}

cocos2d::extension::TableViewCell* NetworkChatView::tableCellAtIndex(cocos2d::extension::TableView *table, ssize_t idx)
{
    const int BG_TAG = 554;
    const int LABEL_TAG = 555;
    const float LABEL_PADDING = 8.f;
    cocos2d::extension::TableViewCell* cell = table->dequeueCell();
    if (!cell)
    {
        cell = cocos2d::extension::TableViewCell::create();
        
        cocos2d::ui::Scale9Sprite* sprite = HUDHelper::createHUDFill();
        sprite->setPreferredSize(GameViewConstants::CHAT_MESSAGE_CELL_SIZE);
        sprite->setPosition(cocos2d::Vec2(GameViewConstants::CHAT_MESSAGE_CELL_SIZE.width * 0.5f,
                                          GameViewConstants::CHAT_MESSAGE_CELL_SIZE.height * 0.5f));
        sprite->setTag(BG_TAG);
        cell->addChild(sprite, -1);
        
        auto label = HUDHelper::createLabel5x7(m_messages.at(m_messages.size() - (idx + 1)), 18);
        label->setPosition(cocos2d::Vec2(LABEL_PADDING, GameViewConstants::CHAT_MESSAGE_CELL_SIZE.height * 0.5f));
        label->setAlignment(cocos2d::TextHAlignment::LEFT, cocos2d::TextVAlignment::CENTER);
        label->setAnchorPoint(cocos2d::Vec2(0.f, 0.5f));
        label->setTag(LABEL_TAG);
        cell->addChild(label);
    }
    else
    {
        auto label = static_cast<cocos2d::Label*>(cell->getChildByTag(LABEL_TAG));
        if (label)
        {
            label->setString(m_messages.at(m_messages.size() - (idx + 1)));
        }
    }
    
    auto sprite = static_cast<cocos2d::Sprite*>(cell->getChildByTag(BG_TAG));
    if (sprite)
    {
        sprite->setColor(idx % 2 ? GameViewConstants::TABLEVIEW_CELL_COLOR : GameViewConstants::TABLEVIEW_CELL_COLOR_ALT);
    }

    return cell;
}

ssize_t NetworkChatView::numberOfCellsInTableView(cocos2d::extension::TableView *table)
{
    return m_messages.size();
}

void NetworkChatView::tableCellTouched(cocos2d::extension::TableView* table,
                                       cocos2d::extension::TableViewCell* cell)
{
    
}

cocos2d::Size NetworkChatView::tableCellSizeForIndex(cocos2d::extension::TableView* table,
                                                     ssize_t idx)
{
    return GameViewConstants::CHAT_MESSAGE_CELL_SIZE;
}

cocos2d::Size NetworkChatView::cellSizeForTable(cocos2d::extension::TableView* table)
{
    return GameViewConstants::CHAT_MESSAGE_CELL_SIZE;
}
