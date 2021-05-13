#ifndef ToggleSlowMotionEvent_h
#define ToggleSlowMotionEvent_h

#include "Core/Event.h"

class ToggleSlowMotionEvent : public Event
{
public:
    ToggleSlowMotionEvent(const bool toggle);
    
    virtual DescriptorType type() const { return descriptor; }

    static constexpr DescriptorType descriptor = "ToggleSlowMotion";
    
    bool getToggle() const { return m_toggle; }
private:
    bool m_toggle;
};

#endif /* ToggleSlowMotionEvent_h */
