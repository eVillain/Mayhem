#ifndef BaseAI_h
#define BaseAI_h

#include "EntityConstants.h"
#include <stdint.h>
#include <memory>

class ClientInputMessage;
class EntitiesModel;
class Entity;
class Player;

class BaseAI
{
public:
    enum State {
        MOVE_TO_TARGET,
        PERFORM_ACTION,
    };
    enum TargetType {
        NONE,
        WEAPON,
        AMMO,
        ENEMY,
        THREAT,
    };
    
    BaseAI();
    
    virtual void update(const float deltaTime,
                        const uint8_t playerID,
                        const std::shared_ptr<EntitiesModel>& entityModel);
    
    std::shared_ptr<ClientInputMessage> getInput() const;

    const State getState() const { return m_state; }
    const TargetType getTargetType() const { return m_targetType; }
    const float getUpdateAccumulator() const { return m_updateAccumulator; }
    
private:
    static const float AI_UPDATE_INTERVAL;
    static const float AI_AWARENESS_RADIUS;

    State m_state;
    TargetType m_targetType;
    float m_updateAccumulator;
    float m_updateTime;

    float m_directionX;
    float m_directionY;
    float m_aimPointX;
    float m_aimPointY;
    bool m_shoot;
    bool m_interact;
    bool m_run;
    bool m_reload;
    bool m_changeWeapon;
    uint8_t m_slot;

    void refreshState(const uint8_t playerID,
                      const std::shared_ptr<EntitiesModel>& entityModel);
    void updateState(const uint8_t playerID,
                     const std::shared_ptr<EntitiesModel>& entityModel);
    void resetState();
    
    cocos2d::Vec2 getClosestOfType(const TargetType type,
                                   const cocos2d::Vec2& position,
                                   const uint16_t ignoreEntityID,
                                   const std::vector<std::shared_ptr<Entity>>& entities,
                                   EntityType ammoType) const;

};

#endif /* BaseAI_h */
