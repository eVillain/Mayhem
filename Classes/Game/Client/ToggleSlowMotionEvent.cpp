#include "ToggleSlowMotionEvent.h"

constexpr ToggleSlowMotionEvent::DescriptorType ToggleSlowMotionEvent::descriptor;

ToggleSlowMotionEvent::ToggleSlowMotionEvent(const bool toggle)
: m_toggle(toggle)
{
}
