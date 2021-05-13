#include "EntityInfoView.h"

#include "HUDHelper.h"
#include "ButtonUtils.h"
#include "GameViewConstants.h"

EntityInfoView::EntityInfoView()
: m_background(nullptr)
, m_border(nullptr)
, m_nameLabel(HUDHelper::createLabel3x6("Name: ", 24))
, m_posLabel(HUDHelper::createLabel3x6("Pos: ", 24))
, m_rotLabel(HUDHelper::createLabel3x6("Rot: ", 24))
, m_velLabel(HUDHelper::createLabel3x6("Vel: ", 24))
, m_angVelLabel(HUDHelper::createLabel3x6("AngVel: ", 24))
, m_ownerLabel(HUDHelper::createLabel3x6("Owner: ", 24))
{
    auto winSize = cocos2d::Director::getInstance()->getWinSize();
    auto box = HUDHelper::createBox(cocos2d::Size(winSize.width * 0.5f, 100.f),
                                    cocos2d::Vec2::ZERO);
    m_background = box.fill;
    m_border = box.border;
    
    addChild(m_background);
    addChild(m_border);
    
    addChild(m_nameLabel);
    addChild(m_posLabel);
    addChild(m_rotLabel);
    addChild(m_velLabel);
    addChild(m_angVelLabel);
    addChild(m_ownerLabel);
}

void EntityInfoView::setContentSize(const cocos2d::Size& size)
{
    Node::setContentSize(size);
    
    m_background->setPreferredSize(size);
    m_border->setPreferredSize(size - cocos2d::Size(2, 2));
    
    m_nameLabel->setAnchorPoint(cocos2d::Vec2::ZERO);
    m_posLabel->setAnchorPoint(cocos2d::Vec2::ZERO);
    m_rotLabel->setAnchorPoint(cocos2d::Vec2::ZERO);
    m_velLabel->setAnchorPoint(cocos2d::Vec2::ZERO);
    m_angVelLabel->setAnchorPoint(cocos2d::Vec2::ZERO);
    m_ownerLabel->setAnchorPoint(cocos2d::Vec2::ZERO);

    m_nameLabel->setPosition(cocos2d::Vec2(8.f, getContentSize().height - 20));
    m_posLabel->setPosition(cocos2d::Vec2(8.f, getContentSize().height - 40));
    m_rotLabel->setPosition(cocos2d::Vec2(8.f, getContentSize().height - 60));
    m_velLabel->setPosition(cocos2d::Vec2(8.f, getContentSize().height - 80));
    m_angVelLabel->setPosition(cocos2d::Vec2(8.f, getContentSize().height - 100));
    m_ownerLabel->setPosition(cocos2d::Vec2(8.f, getContentSize().height - 120));
}
