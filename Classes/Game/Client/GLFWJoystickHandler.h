#ifndef GLFWJoystickHandler_h
#define GLFWJoystickHandler_h

#include <functional>
#include <map>

class GLFWJoystickHandler
{
public:
    struct Gamepad {
        std::string name;
        std::vector<float> axes;
        std::vector<float> buttons;
    };
    
    GLFWJoystickHandler();
    ~GLFWJoystickHandler();
    
    void update();
    
    void setConnectedCallback(std::function<void(int, std::string)> cb) { onConnectedCallback = cb; }
    void setDisconnectedCallback(std::function<void(int)> cb) { onDisconnectedCallback = cb; }
    
    const std::map<int, Gamepad>& getGamepads() const { return m_gamepads; }
    
private:
    std::function<void(int, std::string)> onConnectedCallback;
    std::function<void(int)> onDisconnectedCallback;
    std::map<int, Gamepad> m_gamepads;
    
    void onJoystickConnected(const std::string& name, const int jid);
    void onJoystickDisconnected(const int jid);
    
};

#endif /* GameViewController_h */
