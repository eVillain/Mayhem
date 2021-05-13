#ifndef HUDView_h
#define HUDView_h

#include "HUDWeapon.h"
#include "KillFeed.h"
#include "cocos2d.h"
#include "cocos-ext.h"

class CrosshairView;
class InventoryLayer;
class ViewLayer;

class HUDView
{
public:
    HUDView();
    ~HUDView();
    
    void initialize();

    void setHealth(const float health);
    void setAmmo(const size_t magAmmo, const size_t inventoryAmmo);
    void setPlayersAlive(const size_t playersAlive);
    
    cocos2d::RefPtr<cocos2d::Node> getRoot() { return m_root; }
    cocos2d::RefPtr<cocos2d::DrawNode> getDrawNode() { return m_drawNode; }

    cocos2d::Label* getHighlightLabel() const { return m_highlightLabel; }
    void showHighlightLabel(const std::string& text, const cocos2d::Vec2 position);
    void hideHighlightLabel();
    
    void setWeaponSlot(const size_t slot,
                       const std::string& frame,
                       const bool highlight);

    std::shared_ptr<CrosshairView> getCrosshairView() { return m_crosshair; }

    const cocos2d::RefPtr<KillFeed>& getKillFeed() const { return m_killFeed; }

    void setViewLayer(std::shared_ptr<ViewLayer> layer);
    std::shared_ptr<ViewLayer> getViewLayer();
    void removeViewLayer();
    
    void updatePositions();
    
    void showHealthBlimp(const int health, const cocos2d::Vec2& position);
    
private:
    cocos2d::RefPtr<cocos2d::Node> m_root;
    cocos2d::RefPtr<cocos2d::DrawNode> m_drawNode;
    std::shared_ptr<ViewLayer> m_viewLayer;
    
    cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> m_healthBarBG;
    cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> m_healthBar;
    
    cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> m_ammoBG;
    cocos2d::RefPtr<cocos2d::Label> m_ammoLabel;
    
    cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> m_playersAliveBG;
    cocos2d::RefPtr<cocos2d::Label> m_playersAliveLabel;

    cocos2d::RefPtr<cocos2d::Label> m_highlightLabel;
    std::shared_ptr<CrosshairView> m_crosshair;
    
    std::vector<cocos2d::RefPtr<HUDWeapon>> m_weaponSlots;
    
    cocos2d::RefPtr<KillFeed> m_killFeed;
    
    bool m_blockInput;
};

#endif /* HUDView_h */
