#ifndef InputModel_h
#define InputModel_h

#include "cocos2d.h"

class InputModel
{
public:
    InputModel();
    
    void setDirection(const cocos2d::Vec2& direction) { m_direction = direction; }
    void setMouseCoord(const cocos2d::Vec2 coord) { m_mouseCoord = coord; }
    void setRun(bool run) { m_run = run; }
    void setInteract(bool interact) { m_interact = interact; }
    void setReload(bool reload) { m_reload = reload; }
    void setShoot(const bool shoot) { m_shoot = shoot; }
    void setAim(const bool aim) { m_aim = aim; }
    void setZoomIn(bool zoomIn) { m_zoomIn = zoomIn; }
    void setZoomOut(bool zoomOut) { m_zoomOut = zoomOut; }
    void setSlot(int slot) { m_slot = slot; }
    void setChangeWeapon(bool changeWeapon) { m_changeWeapon = changeWeapon; }

    void setPickupType(uint8_t type) { m_pickupType = type; }
    void setPickupAmount(uint16_t amount) { m_pickupAmount = amount; }
    void setPickupID(uint16_t pickupID) { m_pickupID = pickupID; }

    const cocos2d::Vec2& getDirection() const { return m_direction; }
    const cocos2d::Vec2& getMouseCoord() const { return m_mouseCoord; }
    bool getRun() const { return m_run; }
    bool getInteract() const { return m_interact; }
    bool getReload() const { return m_reload; }
    bool getShoot() const { return m_shoot; }
    bool getAim() const { return m_aim; }
    bool getZoomIn() const { return m_zoomIn; }
    bool getZoomOut() const { return m_zoomOut; }
    
    bool getChangeWeapon() const { return m_changeWeapon; }
    int getSlot() const { return m_slot; }
    
    uint8_t getPickupType() const { return m_pickupType; }
    uint16_t getPickupAmount() const { return m_pickupAmount; }
    uint16_t getPickupID() const { return m_pickupID; }
private:
    cocos2d::Vec2 m_direction;
    cocos2d::Vec2 m_mouseCoord;
    bool m_run;
    bool m_interact;
    bool m_reload;
    bool m_shoot;
    bool m_aim;
    bool m_zoomIn;
    bool m_zoomOut;
    
    bool m_changeWeapon;
    int m_slot;
    
    uint8_t m_pickupType;
    uint16_t m_pickupAmount;
    uint16_t m_pickupID;
};

#endif /* InputModel_h */
