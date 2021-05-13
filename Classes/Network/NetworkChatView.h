#ifndef NETWORK_CHAT_VIEW_H
#define NETWORK_CHAT_VIEW_H

#include "cocos2d.h"
#include "cocos-ext.h"
#include <cocos/ui/UIButton.h>
#include <cocos/ui/UIEditBox/UIEditBox.h>

class NetworkChatView
: public cocos2d::Node
, public cocos2d::extension::TableViewDataSource
, public cocos2d::extension::TableViewDelegate
{
public:
    NetworkChatView();
    
    virtual bool init() override;
    
    void addMessage(const std::string& message, const int senderID);
        
    CREATE_FUNC(NetworkChatView);
    
    cocos2d::extension::TableViewCell* tableCellAtIndex(cocos2d::extension::TableView *table,
                                                        ssize_t idx) override;
    ssize_t numberOfCellsInTableView(cocos2d::extension::TableView *table) override;
    
    void tableCellTouched(cocos2d::extension::TableView* table,
                          cocos2d::extension::TableViewCell* cell) override;
    
    cocos2d::Size tableCellSizeForIndex(cocos2d::extension::TableView* table, ssize_t idx) override;
    
    cocos2d::Size cellSizeForTable(cocos2d::extension::TableView* table) override;
    
    cocos2d::ui::Button* getSendMessageButton() { return m_sendMessageButton; }
    cocos2d::ui::EditBox* getMessageEditBox() { return m_messageEditBox; }

    cocos2d::ui::Scale9Sprite* getMessagesHeaderBG() const { return m_messagesHeaderBG; }
    cocos2d::ui::Scale9Sprite* getMessagesHeaderBorder() const { return  m_messagesHeaderBorder; }
    cocos2d::Label* getMessagesHeaderLabel() const { return  m_messagesHeaderLabel; }
    cocos2d::ui::Scale9Sprite* getMessagesBG() const { return  m_messagesBG; }
    cocos2d::ui::Scale9Sprite* getMessagesBorder() const { return  m_messagesBorder; }

private:
    cocos2d::RefPtr<cocos2d::ui::Button> m_sendMessageButton;
    cocos2d::RefPtr<cocos2d::ui::EditBox> m_messageEditBox;
    
    cocos2d::RefPtr<cocos2d::extension::TableView> m_messagesTableView;
    cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> m_messagesHeaderBG;
    cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> m_messagesHeaderBorder;
    cocos2d::RefPtr<cocos2d::Label> m_messagesHeaderLabel;
    cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> m_messagesBG;
    cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> m_messagesBorder;
    
    std::vector<std::string> m_messages;
};

#endif // NETWORK_CHAT_VIEW_H
