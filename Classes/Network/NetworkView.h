#ifndef NETWORK_VIEW_H
#define NETWORK_VIEW_H

#include "cocos2d.h"
#include <ui/UIButton.h>
#include <extensions/GUI/CCScrollView/CCTableView.h>

class NetworkChatView;

class NetworkView : public cocos2d::Layer
{
public:
    enum Mode {
        HOST,
        CLIENT,
    };
    
    virtual bool init();
    
    static NetworkView* create(const NetworkView::Mode mode)
    {
        NetworkView *pRet = new(std::nothrow) NetworkView(mode);
        if (pRet && pRet->init())
        {
            pRet->autorelease();
            return pRet;
        }
        else
        {
            delete pRet;
            pRet = nullptr;
            return nullptr;
        }
    }
    
    const cocos2d::RefPtr<cocos2d::ui::Button>& getReadyButton() const { return m_readyButton; }
    const cocos2d::RefPtr<cocos2d::ui::Button>& getExitButton() const { return m_exitButton; }
    NetworkChatView* getChatView() const { return m_chatView; }

    void showReadyButton(const bool show);
    void showExitButton(const bool show);
    
    void setupChatView();
    void removeChatView();

protected:
    NetworkView::Mode m_mode;
    cocos2d::RefPtr<cocos2d::Label> m_title;
    cocos2d::RefPtr<cocos2d::ui::Button> m_readyButton;
    cocos2d::RefPtr<cocos2d::ui::Button> m_exitButton;

    NetworkChatView* m_chatView;
    
    NetworkView(const NetworkView::Mode mode);
    void setupDefaultButtons();
};

#endif // NETWORK_VIEW_H
