#include "HUDView.h"
#include "HUDHelper.h"

#include "CrosshairView.h"
#include "Core/Dispatcher.h"
#include "GameViewConstants.h"
#include "Core/Injector.h"
#include "InventoryLayer.h"
#include "ViewLayer.h"
#include "KillFeed.h"

static const cocos2d::Size HEALTH_BAR_SIZE = cocos2d::Size(200.f, 26.f);

HUDView::HUDView()
: m_root(cocos2d::Node::create())
, m_drawNode(cocos2d::DrawNode::create())
, m_viewLayer(nullptr)
, m_healthBarBG(nullptr)
, m_healthBar(nullptr)
, m_ammoBG(nullptr)
, m_ammoLabel(nullptr)
, m_playersAliveBG(nullptr)
, m_playersAliveLabel(nullptr)
, m_highlightLabel(nullptr)
, m_crosshair(nullptr)
, m_killFeed(nullptr)
, m_blockInput(false)
{
}

HUDView::~HUDView()
{
}

void HUDView::initialize()
{
    cocos2d::SpriteFrameCache* cache = cocos2d::SpriteFrameCache::getInstance();
    cache->addSpriteFramesWithFile(GameViewConstants::HUD_SPRITE_BATCH_FILE);
        
    m_healthBarBG = HUDHelper::createHUDFill();
    m_healthBarBG->setAnchorPoint(cocos2d::Vec2::ZERO);
    m_healthBarBG->setPreferredSize(HEALTH_BAR_SIZE + cocos2d::Size(4.f, 4.f));
    m_healthBarBG->setColor(cocos2d::Color3B::GRAY);
    m_healthBarBG->setOpacity(127);
    m_healthBar = HUDHelper::createHUDFill();
    m_healthBar->setPreferredSize(HEALTH_BAR_SIZE);
    m_healthBar->setAnchorPoint(cocos2d::Vec2::ZERO);
    
    m_playersAliveBG = HUDHelper::createHUDFill();
    m_playersAliveBG->setPreferredSize(cocos2d::Size(162, 32));
    m_playersAliveBG->setColor(cocos2d::Color3B::BLACK);
    m_playersAliveBG->setOpacity(127);

    m_playersAliveLabel = HUDHelper::createLabel5x7("## | Alive", 48);

    m_ammoBG = HUDHelper::createHUDFill();
    m_ammoBG->setPreferredSize(cocos2d::Size(182, 32));
    m_ammoBG->setColor(cocos2d::Color3B::BLACK);
    m_ammoBG->setOpacity(127);
    
    m_ammoLabel = HUDHelper::createLabel5x7("## | ##", 48);
    
    m_highlightLabel = HUDHelper::createLabel5x7("Hello!", 24);
    m_highlightLabel->setAlignment(cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
    m_highlightLabel->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
    m_highlightLabel->setOpacity(0);
    
    m_crosshair = std::make_shared<CrosshairView>();
    m_crosshair->initialize();
    
    m_killFeed = KillFeed::create();
    
    m_root->addChild(m_healthBarBG);
    m_root->addChild(m_healthBar);
    
    m_root->addChild(m_ammoBG);
    m_root->addChild(m_ammoLabel);

    m_root->addChild(m_playersAliveBG);
    m_root->addChild(m_playersAliveLabel);
    
    for (size_t i = 0; i < 5; i++)
    {
        cocos2d::RefPtr<HUDWeapon> weaponSlot = HUDWeapon::create();
        m_root->addChild(weaponSlot);
        m_weaponSlots.push_back(weaponSlot);
    }
    
    m_root->addChild(m_drawNode);
    m_root->addChild(m_highlightLabel, 100);
    m_root->addChild(m_crosshair->getCrossHair(), 200);
    
    m_root->addChild(m_killFeed);
    
    setHealth(100.f);
    updatePositions();    
}

void HUDView::setHealth(const float health)
{
    const cocos2d::Size oldSize = m_healthBar->getPreferredSize();
    const cocos2d::Size newSize = cocos2d::Size(health * 2.f, HEALTH_BAR_SIZE.height);
 
    if (oldSize.width == newSize.width)
    {
        return;
    }
    m_healthBar->setPreferredSize(newSize);

    bool increase = oldSize.width < newSize.width;
    const float widthChange = std::max(newSize.width - oldSize.width, oldSize.width - newSize.width);
    auto healthChangeSprite = HUDHelper::createHUDFill();
    healthChangeSprite->setAnchorPoint(cocos2d::Vec2::ZERO);
    healthChangeSprite->setColor(increase ? cocos2d::Color3B::GREEN : cocos2d::Color3B::RED);
    healthChangeSprite->setPreferredSize(cocos2d::Size(widthChange, 26.f));
    healthChangeSprite->setPositionX(m_healthBar->getPositionX() + (increase ? oldSize.width : newSize.width));
    healthChangeSprite->setPositionY(m_healthBar->getPositionY());
    m_root->addChild(healthChangeSprite, 100.f);
    
    float changeDuration = (widthChange / HEALTH_BAR_SIZE.width);
    cocos2d::FadeOut* fadeOut = cocos2d::FadeOut::create(changeDuration);
    cocos2d::ResizeTo* resize = cocos2d::ResizeTo::create(changeDuration, cocos2d::Size(1.f, HEALTH_BAR_SIZE.height));
    cocos2d::RemoveSelf* remove = cocos2d::RemoveSelf::create();
    cocos2d::Sequence* healthChangeSequence = cocos2d::Sequence::create(cocos2d::Spawn::createWithTwoActions(fadeOut, resize), remove, NULL);
    healthChangeSprite->runAction(healthChangeSequence);
}

void HUDView::setAmmo(const size_t magAmmo, const size_t inventoryAmmo)
{
    m_ammoLabel->setString(std::to_string(magAmmo) + " | " + std::to_string(inventoryAmmo));
}

void HUDView::setPlayersAlive(const size_t playersAlive)
{
    m_playersAliveLabel->setString(std::to_string(playersAlive) + " | Alive");
}

void HUDView::showHighlightLabel(const std::string& text, const cocos2d::Vec2 position)
{
    if (m_highlightLabel->getString() == text &&
        m_highlightLabel->getPosition() == position)
    {
        return;
    }
    m_highlightLabel->setString(text);
    m_highlightLabel->setPosition(position);
    m_highlightLabel->setOpacity(255);
}

void HUDView::hideHighlightLabel()
{
    if (m_highlightLabel->getOpacity() == 0)
    {
        return;
    }
    m_highlightLabel->setOpacity(0);
    m_highlightLabel->setString("");
}

void HUDView::setWeaponSlot(const size_t slot,
                            const std::string& frame,
                            const bool highlight)
{
    m_weaponSlots.at(slot)->setup(frame, highlight);
}

void HUDView::setViewLayer(std::shared_ptr<ViewLayer> layer)
{
    if (m_viewLayer)
    {
        removeViewLayer();
    }
    
    m_viewLayer = layer;
    
    if (m_viewLayer)
    {
        auto director = cocos2d::Director::getInstance();
        auto glview = director->getOpenGLView();
        glview->setCursorVisible(true);
        m_root->addChild(m_viewLayer->getRoot(), 201);
    }
}

std::shared_ptr<ViewLayer> HUDView::getViewLayer()
{
    return m_viewLayer;
}

void HUDView::removeViewLayer()
{
    if (!m_viewLayer)
    {
        return;
    }
    auto director = cocos2d::Director::getInstance();
    auto glview = director->getOpenGLView();
    glview->setCursorVisible(false);
    m_viewLayer->getRoot()->removeFromParent();
    m_viewLayer = nullptr;
}

void HUDView::updatePositions()
{
    const float PADDING = 20.f;
    const cocos2d::Size windowSize = cocos2d::Director::getInstance()->getWinSize();
    m_healthBar->setPosition(cocos2d::Vec2((windowSize.width * 0.5f) - (HEALTH_BAR_SIZE.width * 0.5f), 22.f));
    m_healthBarBG->setPosition(cocos2d::Vec2((windowSize.width - m_healthBarBG->getPreferredSize().width) * 0.5f, 20.f));
    
    m_playersAliveLabel->setPosition(windowSize - (m_playersAliveLabel->getContentSize() + cocos2d::Size(PADDING - 2, PADDING - 2)));
    m_playersAliveBG->setPosition(windowSize - (m_playersAliveLabel->getContentSize() + cocos2d::Size(PADDING, PADDING)));
    
    m_ammoLabel->setPosition(cocos2d::Vec2(windowSize.width * 0.5f, 65.f));
    m_ammoBG->setPosition(cocos2d::Vec2(windowSize.width * 0.5f, 65.f));
    
    for (size_t i = 0; i < 5; i++)
    {
        cocos2d::RefPtr<HUDWeapon>& weaponSlot = m_weaponSlots.at(i);
        weaponSlot->setPosition(cocos2d::Vec2(windowSize.width - GameViewConstants::HUD_WEAPON_SIZE.width, 100 - (i * 20)));
    }
    
    if (m_viewLayer)
    {
        m_viewLayer->update(0.f);
    }
}

void HUDView::showHealthBlimp(const int health, const cocos2d::Vec2& position)
{
    auto label = HUDHelper::createLabel3x6(std::to_string(health), 48);
    label->setTextColor(health > 0 ? cocos2d::Color4B::GREEN : cocos2d::Color4B::RED);
    label->setScale(0.1f);
    label->setPosition(position);
    label->enableOutline(cocos2d::Color4B::BLACK, 2);
    m_root->addChild(label, 5000);
    
    auto grow = cocos2d::ScaleTo::create(0.2f, 1.f);
    auto flyUp = cocos2d::MoveBy::create(1.f, cocos2d::Vec2(0.f, 50.f));
    auto fadeOut = cocos2d::FadeOut::create(0.3f);
    auto removeCallback = cocos2d::CallFunc::create([label](){
        label->removeFromParent();
    });
    auto sequence = cocos2d::Sequence::create(grow, flyUp, fadeOut, removeCallback, NULL);
    label->runAction(sequence);
}
