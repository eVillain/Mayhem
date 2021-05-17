#include "Game/Client/InputModel.h"

InputModel::InputModel()
: m_direction(cocos2d::Vec2::ZERO)
, m_mouseCoord(cocos2d::Vec2::ZERO)
, m_run(false)
, m_interact(false)
, m_reload(false)
, m_shoot(false)
, m_zoomIn(false)
, m_zoomOut(false)
, m_changeWeapon(false)
, m_slot(0)
, m_pickupType(0)
, m_pickupAmount(0)
, m_pickupID(0)
{
    printf("InputModel:: constructor: %p\n", this);
}

InputModel::~InputModel()
{
    printf("InputModel:: destructor: %p\n", this);
}
