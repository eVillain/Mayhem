#include "HUDView.h"


#include "CrosshairView.h"
#include "GameViewConstants.h"
#include "Core/Injector.h"
#include "HUDConstants.h"
#include "HUDHelper.h"
#include "InventoryLayer.h"
#include "ViewLayer.h"
#include "KillFeed.h"
#include "EntityDataModel.h"
#include "NumberFormatter.h"

static const cocos2d::Size HEALTH_BAR_SIZE = cocos2d::Size(200.f, 26.f);

HUDView::HUDView()
: m_root(cocos2d::Node::create())
, m_drawNode(cocos2d::DrawNode::create())
, m_viewLayer(nullptr)
, m_healthBarBG(nullptr)
, m_healthBar(nullptr)
, m_ammoBG(nullptr)
, m_ammoLabel(nullptr)
, m_killsLabel(nullptr)
, m_playersAliveLabel(nullptr)
, m_teamsAliveLabel(nullptr)
, m_spectatorsLabel(nullptr)
, m_highlightLabel(nullptr)
, m_killsIcon(nullptr)
, m_playersIcon(nullptr)
, m_teamsIcon(nullptr)
, m_spectatorsIcon(nullptr)
, m_crosshair(nullptr)
, m_killFeed(nullptr)
, m_blockInput(false)
{
    printf("HUDView:: constructor: %p\n", this);
}

HUDView::~HUDView()
{
    printf("HUDView:: destructor: %p\n", this);
}

void HUDView::initialize()
{
    cocos2d::SpriteFrameCache* cache = cocos2d::SpriteFrameCache::getInstance();
    cache->addSpriteFramesWithFile(HUDConstants::HUD_SPRITE_BATCH_FILE);
        
    m_healthBarBG = HUDHelper::createHUDFill();
    m_healthBarBG->setAnchorPoint(cocos2d::Vec2::ZERO);
    m_healthBarBG->setPreferredSize(HEALTH_BAR_SIZE + cocos2d::Size(4.f, 4.f));
    m_healthBarBG->setColor(cocos2d::Color3B::GRAY);
    m_healthBarBG->setOpacity(127);
    m_healthBar = HUDHelper::createHUDFill();
    m_healthBar->setPreferredSize(HEALTH_BAR_SIZE);
    m_healthBar->setAnchorPoint(cocos2d::Vec2::ZERO);

    m_killsLabel = HUDHelper::createLabel5x7("##", 32);
    m_killsLabel->setAlignment(cocos2d::TextHAlignment::LEFT);
    m_killsLabel->setAnchorPoint(cocos2d::Vec2::ANCHOR_TOP_LEFT);
    m_killsLabel->enableOutline(cocos2d::Color4B::BLACK, 1);
    m_playersAliveLabel = HUDHelper::createLabel5x7("##", 32);
    m_playersAliveLabel->setAlignment(cocos2d::TextHAlignment::LEFT);
    m_playersAliveLabel->setAnchorPoint(cocos2d::Vec2::ANCHOR_TOP_LEFT);
    m_playersAliveLabel->enableOutline(cocos2d::Color4B::BLACK, 1);
    m_teamsAliveLabel = HUDHelper::createLabel5x7("##", 32);
    m_teamsAliveLabel->setAlignment(cocos2d::TextHAlignment::LEFT);
    m_teamsAliveLabel->setAnchorPoint(cocos2d::Vec2::ANCHOR_TOP_LEFT);
    m_teamsAliveLabel->enableOutline(cocos2d::Color4B::BLACK, 1);
    m_spectatorsLabel = HUDHelper::createLabel5x7("##", 32);
    m_spectatorsLabel->setAlignment(cocos2d::TextHAlignment::LEFT);
    m_spectatorsLabel->setAnchorPoint(cocos2d::Vec2::ANCHOR_TOP_LEFT);
    m_spectatorsLabel->enableOutline(cocos2d::Color4B::BLACK, 1);

    m_killsIcon = cocos2d::Sprite::createWithSpriteFrameName("Icon-Kills.png");
    m_killsIcon->setAnchorPoint(cocos2d::Vec2::ANCHOR_TOP_LEFT);
    m_playersIcon = cocos2d::Sprite::createWithSpriteFrameName("Icon-Players.png");
    m_playersIcon->setAnchorPoint(cocos2d::Vec2::ANCHOR_TOP_LEFT);
    m_teamsIcon = cocos2d::Sprite::createWithSpriteFrameName("Icon-Teams.png");
    m_teamsIcon->setAnchorPoint(cocos2d::Vec2::ANCHOR_TOP_LEFT);
    m_spectatorsIcon = cocos2d::Sprite::createWithSpriteFrameName("Icon-Spectators.png");
    m_spectatorsIcon->setAnchorPoint(cocos2d::Vec2::ANCHOR_TOP_LEFT);

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

    m_root->addChild(m_killsLabel);
    m_root->addChild(m_playersAliveLabel);
    m_root->addChild(m_teamsAliveLabel);
    m_root->addChild(m_spectatorsLabel);

    m_root->addChild(m_killsIcon);
    m_root->addChild(m_playersIcon);
    m_root->addChild(m_teamsIcon);
    m_root->addChild(m_spectatorsIcon);
    
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

void HUDView::shutdown()
{
    m_root->removeAllChildren();
    m_viewLayer = nullptr;
    m_healthBarBG = nullptr;
    m_healthBar = nullptr;
    m_ammoBG = nullptr;
    m_ammoLabel = nullptr;
    m_playersAliveLabel = nullptr;
    m_teamsAliveLabel = nullptr;
    m_killsLabel = nullptr;
    m_spectatorsLabel = nullptr;
    m_killsIcon = nullptr;
    m_playersIcon = nullptr;
    m_teamsIcon = nullptr;
    m_spectatorsIcon = nullptr;
    m_highlightLabel = nullptr;
    m_crosshair = nullptr;
    m_killFeed = nullptr;
    m_blockInput = nullptr;
}

void HUDView::update(const SnapshotData &snapshot, const uint8_t localPlayerID)
{
    uint8_t kills = 0;
    size_t playersAlive = 0;
    uint16_t inventoryAmmo = 0;
    uint16_t weaponAmmo = 0;
    float health = 0.f;
    
    const auto& localPlayerIt = snapshot.playerData.find(localPlayerID);
    if (localPlayerIt != snapshot.playerData.end())
    {
        const PlayerState& playerState = localPlayerIt->second;
        health = playerState.health;
        kills = playerState.kills;
        
        const WeaponSlot activeSlot = (WeaponSlot)playerState.activeWeaponSlot;
        for (size_t i = 0; i < 5; i++)
        {
            bool isActiveSlot = i == activeSlot;
            const auto& slotWeapon = playerState.weaponSlots.at(i);
            std::string weaponSpriteFrame;
            const bool hasWeapon = slotWeapon.type != EntityType::NoEntity;
            if (hasWeapon)
            {
                const StaticEntityData& slotWeaponData = EntityDataModel::getStaticEntityData((EntityType)slotWeapon.type);
                if (isActiveSlot)
                {
                    weaponAmmo = slotWeapon.amount;
                    auto it = std::find_if(snapshot.inventory.begin(),
                                           snapshot.inventory.end(),
                                           [&slotWeaponData](const InventoryItemState& itemState) {
                        return itemState.type == slotWeaponData.ammo.type;
                    });
                    if (it != snapshot.inventory.end())
                    {
                        inventoryAmmo = (*it).amount;
                    }
                }
                weaponSpriteFrame = slotWeaponData.sprite;
            }

            setWeaponSlot(i, weaponSpriteFrame, isActiveSlot);
        }
    }
    
    for (const auto& player : snapshot.playerData)
    {
        if (player.second.health > 0.f)
        {
            playersAlive++;
        }
    }

    setKills(kills);
    setPlayersAlive(playersAlive);
    setAmmo(weaponAmmo, inventoryAmmo);
    setHealth(health);
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
    m_playersAliveLabel->setString(NumberFormatter::toString(playersAlive, 0));
}

void HUDView::setTeamsAlive(const size_t teamsAlive)
{
    m_teamsAliveLabel->setString(NumberFormatter::toString(teamsAlive, 0));
}

void HUDView::setKills(const size_t kills)
{
    m_killsLabel->setString(NumberFormatter::toString(kills, 0));
}

void HUDView::setSpectators(const size_t spectators)
{
    m_spectatorsLabel->setString(NumberFormatter::toString(spectators, 0));
}

void HUDView::setTeamsVisible(const bool visible)
{
    m_teamsAliveLabel->setVisible(visible);
    m_teamsIcon->setVisible(visible);
}

void HUDView::setSpectatorsVisible(const bool visible)
{
    m_spectatorsLabel->setVisible(visible);
    m_spectatorsIcon->setVisible(visible);
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
    const float PADDING = 8.f;
    const cocos2d::Size windowSize = cocos2d::Director::getInstance()->getWinSize();
    m_healthBar->setPosition(cocos2d::Vec2((windowSize.width * 0.5f) - (HEALTH_BAR_SIZE.width * 0.5f), 22.f));
    m_healthBarBG->setPosition(cocos2d::Vec2((windowSize.width - m_healthBarBG->getPreferredSize().width) * 0.5f, 20.f));
    
    const float LABEL_SIZE = 20.f;
    const float ICON_SIZE = 32.f;
    // Top HUD labels, they go at the top-right
    cocos2d::Vec2 labelPos = windowSize - cocos2d::Size(LABEL_SIZE + PADDING, PADDING);
    m_killsLabel->setPosition(labelPos);
    m_killsIcon->setPosition(labelPos - cocos2d::Vec2(ICON_SIZE, 0.f));
    labelPos -= cocos2d::Vec2(LABEL_SIZE + ICON_SIZE + PADDING, 0.f);
    m_playersAliveLabel->setPosition(labelPos);
    m_playersIcon->setPosition(labelPos - cocos2d::Vec2(ICON_SIZE, 0.f));
    if (m_teamsAliveLabel->isVisible())
    {
        labelPos -= cocos2d::Vec2(LABEL_SIZE + ICON_SIZE + PADDING, 0.f);
        m_teamsAliveLabel->setPosition(labelPos);
        m_teamsIcon->setPosition(labelPos - cocos2d::Vec2(ICON_SIZE, 0.f));
    }
    if (m_spectatorsLabel->isVisible())
    {
        labelPos -= cocos2d::Vec2(LABEL_SIZE + ICON_SIZE + PADDING, 0.f);
        m_spectatorsLabel->setPosition(labelPos);
        m_spectatorsIcon->setPosition(labelPos - cocos2d::Vec2(ICON_SIZE, 0.f));
    }

    m_ammoLabel->setPosition(cocos2d::Vec2(windowSize.width * 0.5f, 65.f));
    m_ammoBG->setPosition(cocos2d::Vec2(windowSize.width * 0.5f, 65.f));
    
    for (size_t i = 0; i < 5; i++)
    {
        cocos2d::RefPtr<HUDWeapon>& weaponSlot = m_weaponSlots.at(i);
        weaponSlot->setPosition(cocos2d::Vec2(windowSize.width - GameViewConstants::HUD_WEAPON_SIZE.width, 100 - (i * 20)));
    }
    
    m_killFeed->setPosition(cocos2d::Vec2(PADDING, windowSize.height * 0.5));
    
    if (m_viewLayer)
    {
        m_viewLayer->update(0.f);
    }
}

void HUDView::showHealthBlimp(const int health, const cocos2d::Vec2& position)
{
    auto label = HUDHelper::createLabel3x6(NumberFormatter::toString(health, 0), 48);
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
