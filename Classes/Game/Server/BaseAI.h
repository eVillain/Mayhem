#ifndef BaseAI_h
#define BaseAI_h

#include <stdint.h>
#include <memory>

class ClientInputMessage;
class EntitiesModel;

class BaseAI
{
public:
    enum State {
        SEEK_TARGET,
        DO_ACTION,
    };
    enum TargetType {
        NONE,
        WEAPON,
        AMMO,
        ENEMY,
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
    State m_state;
    TargetType m_targetType;
    float m_updateAccumulator;
    
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

    void updateState(const uint8_t playerID,
                     const std::shared_ptr<EntitiesModel>& entityModel);

};

#endif /* BaseAI_h */
