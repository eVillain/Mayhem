#include "GLFWJoystickHandler.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
#include "glfw3.h"

GLFWJoystickHandler::GLFWJoystickHandler()
: onConnectedCallback(nullptr)
, onDisconnectedCallback(nullptr)
{
}

GLFWJoystickHandler::~GLFWJoystickHandler()
{
    glfwSetJoystickCallback(NULL);
}

void GLFWJoystickHandler::update()
{
    for (int jid = 0; jid < 16; jid++)
    {
        if (glfwJoystickPresent(jid))
        {
            const char* name = glfwGetJoystickName(jid);
            const std::string deviceName = name ? name : "";
            onJoystickConnected(deviceName, jid);
        }
        else
        {
            onJoystickDisconnected(jid);
        }
    }
}

void GLFWJoystickHandler::onJoystickConnected(const std::string& name, const int jid)
{
    if (!glfwJoystickIsGamepad(jid))
    {
        onJoystickDisconnected(jid);
        return;
    }
    
    GLFWgamepadstate state;
    if (!glfwGetGamepadState(jid, &state))
    {
        return;
    }
    
    const char* gamepadName = glfwGetGamepadName(jid);
    std::string padName = gamepadName;
    
    auto it = m_gamepads.find(jid);
    if (it == m_gamepads.end())
    {
        Gamepad gamepad = { padName };
        for (int axis = 0; axis < 6; axis++)
        {
            gamepad.axes.push_back(state.axes[axis]);
        }
        for (int button = 0; button < 15; button++)
        {
            gamepad.buttons.push_back(state.buttons[button]);
        }
        m_gamepads[jid] = gamepad;
        
        if (onConnectedCallback)
        {
            onConnectedCallback(jid, padName);
        }
    }
    else
    {
        Gamepad& gamepad = m_gamepads[jid];
        for (int axis = 0; axis < 6; axis++)
        {
            gamepad.axes[axis] = (state.axes[axis]);
        }
        for (int button = 0; button < 15; button++)
        {
            gamepad.buttons[button] = (state.buttons[button]);
        }
    }
}

void GLFWJoystickHandler::onJoystickDisconnected(const int jid)
{
    auto git = m_gamepads.find(jid);
    if (git == m_gamepads.end())
    {
        return;
    }

    m_gamepads.erase(git);      // Controller was a gamepad, remove state

    if (onDisconnectedCallback)
    {
        onDisconnectedCallback(jid);
    }
}

#endif // #if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
