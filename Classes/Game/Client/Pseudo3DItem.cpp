#include "Pseudo3DItem.h"

Pseudo3DItem::Pseudo3DItem(const cocos2d::Vec2 position,
                           const float pseudoZPosition,
                           const cocos2d::Vec2 velocity,
                           const float pseudoZVelocity,
                           const float lifeTime,
                           const float elasticity)
: m_position(position)
, m_pseudoZPosition(pseudoZPosition)
, m_velocity(velocity)
, m_pseudoZVelocity(pseudoZVelocity)
, m_lifeTime(lifeTime)
, m_timeAlive(0.f)
, m_elasticity(elasticity)
{
}

void Pseudo3DItem::update(const float deltaTime)
{
    const float DAMPING = 0.85f;
    const float GRAVITY = 128.f;
    const float MAX_Z_VELOCITY = 256.f;
    
    m_timeAlive += deltaTime;
    
    if (m_timeAlive >= m_lifeTime)
    {
        return;
    }
    
    if (m_velocity != cocos2d::Vec2::ZERO)
    {
        m_position += m_velocity * deltaTime;
    }
    
    if (m_pseudoZPosition == 0.f)
    {
        m_velocity *= DAMPING;
    }
    else
    {
        m_pseudoZVelocity -= GRAVITY * deltaTime;
        m_pseudoZVelocity = std::min(std::max(m_pseudoZVelocity, -MAX_Z_VELOCITY), MAX_Z_VELOCITY);
        
        if (std::abs(m_pseudoZVelocity) <= 2.f &&
            m_pseudoZPosition <= 1.f)
        {
            m_pseudoZPosition = 0.f;
            m_pseudoZVelocity = 0.f;
        }
        else if (m_pseudoZVelocity != 0.f)
        {
            m_pseudoZPosition += m_pseudoZVelocity * deltaTime;
            if (m_pseudoZPosition < 0.f) // bounce
            {
                if (m_elasticity > 0.f)
                {
                    m_pseudoZPosition = -m_pseudoZPosition * m_elasticity;
                    m_pseudoZVelocity *= -m_elasticity;
                }
                else
                {
                    m_pseudoZPosition = 0.f;
                    m_pseudoZVelocity = 0.f;
                }
            }
        }
    }

}

cocos2d::Vec2 Pseudo3DItem::getScreenPosition() const
{
    return m_position + cocos2d::Vec2(0, m_pseudoZPosition);
}
