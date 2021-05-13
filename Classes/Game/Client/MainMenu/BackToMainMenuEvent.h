#ifndef BackToMainMenuEvent_h
#define BackToMainMenuEvent_h

#include "Core/Event.h"

class Player;
class Entity;

class BackToMainMenuEvent : public Event
{
public:
    BackToMainMenuEvent();
    
    virtual DescriptorType type() const { return descriptor; }

    static constexpr DescriptorType descriptor = "BackToMainMenu";
};

#endif /* BackToMainMenuEvent_h */
