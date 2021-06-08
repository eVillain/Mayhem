#ifndef InputModel_h
#define InputModel_h

#include "cocos2d.h"
#include "InputConstants.h"

struct InputAction {
    std::string action;
    float value;
};

class InputModel
{
public:
    InputModel();
    ~InputModel();

    void setInputValue(const std::string& input, const float value);
    float getInputValue(const std::string& input);

    void mapKeyboard(const cocos2d::EventKeyboard::KeyCode key,
                     const std::string& action,
                     const float value);
    void mapMouseButton(const cocos2d::EventMouse::MouseButton button,
                        const std::string& action,
                        const float value);
    void mapMouseAxis(const int axis,
                      const std::string& action,
                      const float value);
    void mapControllerButton(const int button,
                             const std::string& action,
                             const float value);
    void mapControllerAxis(const int axis,
                           const std::string& action,
                           const float value);
    
    void setActiveGamepad(const int gamepad) { m_activeGamepad = gamepad; }
    int getActiveGamepad() const { return m_activeGamepad; }
    void setActiveGamepadType(GamepadType type) { m_activeGamepadType = type; }
    GamepadType const getActiveGamepadType() const { return m_activeGamepadType; }

    void setMouseCoord(const cocos2d::Vec2 coord) { m_mouseCoord = coord; }

    void setChangeWeapon(bool changeWeapon) { m_changeWeapon = changeWeapon; }
    void setSlot(int slot) { m_slot = slot; }

    void setPickupType(uint8_t type) { m_pickupType = type; }
    void setPickupAmount(uint16_t amount) { m_pickupAmount = amount; }
    void setPickupID(uint16_t pickupID) { m_pickupID = pickupID; }

    const cocos2d::Vec2& getMouseCoord() const { return m_mouseCoord; }

    bool getChangeWeapon() const { return m_changeWeapon; }
    int getSlot() const { return m_slot; }
    
    uint8_t getPickupType() const { return m_pickupType; }
    uint16_t getPickupAmount() const { return m_pickupAmount; }
    uint16_t getPickupID() const { return m_pickupID; }

    const std::map<cocos2d::EventKeyboard::KeyCode, InputAction>& getKeyboardMap() const { return m_keyboardMappings; }
    const std::map<cocos2d::EventMouse::MouseButton, InputAction>& getMouseButtonMap() const { return m_mouseButtonMappings; }
    const std::map<int, InputAction>& getMouseAxisMap() const { return m_mouseAxisMappings; }
    const std::map<int, InputAction>& getControllerButtonMap() const { return m_controllerButtonMappings; }
    const std::map<int, InputAction>& getControllerAxisMap() const { return m_controllerAxisMappings; }

private:
    cocos2d::Vec2 m_mouseCoord;
    
    bool m_changeWeapon;
    int m_slot;
    
    uint8_t m_pickupType;
    uint16_t m_pickupAmount;
    uint16_t m_pickupID;
    
    std::map<cocos2d::EventKeyboard::KeyCode, InputAction> m_keyboardMappings;
    std::map<cocos2d::EventMouse::MouseButton, InputAction> m_mouseButtonMappings;
    std::map<int, InputAction> m_mouseAxisMappings;
    std::map<int, InputAction> m_controllerButtonMappings;
    std::map<int, InputAction> m_controllerAxisMappings;

    std::map<std::string, float> m_inputs;
    
    int m_activeGamepad;
    GamepadType m_activeGamepadType;
};

#endif /* InputModel_h */
