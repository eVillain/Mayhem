#include "Pseudo3DSprite.h"

void Pseudo3DSprite::update(const float deltaTime)
{
    Pseudo3DItem::update(deltaTime);
    
    if (!m_sprite)
    {
        return;
    }
    
    m_sprite->setPosition(getScreenPosition());
}

