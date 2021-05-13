#include "DestroyEntityEvent.h"

constexpr DestroyEntityEvent::DescriptorType DestroyEntityEvent::descriptor;

DestroyEntityEvent::DestroyEntityEvent(const uint16_t entityID)
: m_entityID(entityID)
{
}
