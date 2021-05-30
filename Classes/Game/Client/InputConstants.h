#ifndef InputConstants_h
#define InputConstants_h

#include "cocos2d.h"

namespace InputConstants
{
    static const std::string ACTION_MOVE_RIGHT = "MoveRight";
    static const std::string ACTION_MOVE_UP = "MoveUp";
    static const std::string ACTION_AIM_LEFT = "AimLeft";
    static const std::string ACTION_AIM_UP = "AimUp";
    static const std::string ACTION_SHOOT = "Shoot";
    static const std::string ACTION_AIM = "Aim";
    static const std::string ACTION_RUN = "Run";
    static const std::string ACTION_RELOAD = "Reload";
    static const std::string ACTION_INTERACT = "Interact";
    static const std::string ACTION_BACK = "Back";
    static const std::string ACTION_ZOOM_IN = "ZoomIn";
    static const std::string ACTION_ZOOM_OUT = "ZoomOut";

    static const std::string ACTION_INVENTORY = "Inventory";
    static const std::string ACTION_RENDER_DEBUG = "RenderDebug";
    static const std::string ACTION_CLIENT_PREDICTION = "ClientPrediction";
};

#endif /* InputConstants_h */
