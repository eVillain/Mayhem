#include "Pseudo3DParticle.h"


//void Pseudo3DParticle::update(const float deltaTime)
//{
//    const float DAMPING = 0.85f;
//    const float GRAVITY = 128.f;
//    const float MAX_Z_VELOCITY = 256.f;
//    
//    m_timeAlive += deltaTime;
//    
//    if (m_timeAlive >= m_lifeTime)
//    {
//        return;
//    }
//    
//    if (m_velocity != cocos2d::Vec2::ZERO)
//    {
//        m_position += m_velocity * deltaTime;
//    }
//    
//    if (m_pseudoZPosition == 0.f)
//    {
//        m_velocity *= DAMPING;
//    }
//    else
//    {
//        m_pseudoZVelocity -= GRAVITY * deltaTime;
//        m_pseudoZVelocity = std::min(std::max(m_pseudoZVelocity, -MAX_Z_VELOCITY), MAX_Z_VELOCITY);
//        
//        if (std::abs(m_pseudoZVelocity) <= 2.f &&
//            m_pseudoZPosition <= 1.f)
//        {
//            m_pseudoZPosition = 0.f;
//            m_pseudoZVelocity = 0.f;
//        }
//        else if (m_pseudoZVelocity != 0.f)
//        {
//            m_pseudoZPosition += m_pseudoZVelocity * deltaTime;
//            if (m_pseudoZPosition < 0.f) // bounce
//            {
//                if (m_bouncy)
//                {
//                    m_pseudoZPosition = -m_pseudoZPosition * ELASTICITY;
//                    m_pseudoZVelocity *= -ELASTICITY;
//                }
//                else
//                {
//                    m_pseudoZPosition = 0.f;
//                    m_pseudoZVelocity = 0.f;
//                }
//            }
//        }
//    }
//}

