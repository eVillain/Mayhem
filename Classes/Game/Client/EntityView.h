#ifndef EntityView_h
#define EntityView_h

#include "Game/Shared/EntityConstants.h"
#include "cocos2d.h"
#include <memory>

class EntityView
{
public:
    static const int ANIMATION_TAG;
    static const size_t HEAD_INDEX;
    static const size_t ARM_LEFT_INDEX;
    static const size_t ARM_RIGHT_INDEX;
    static const size_t WEAPON_INDEX;
    static const cocos2d::Vec2 HEAD_POS;
    static const cocos2d::Vec2 ARM_L_POS;
    static const cocos2d::Vec2 ARM_L_POS_FLIPPED;
    static const cocos2d::Vec2 ARM_L_ANCHOR;
    static const cocos2d::Vec2 ARM_L_ANCHOR_FLIPPED;
    static const cocos2d::Vec2 ARM_R_POS;
    static const cocos2d::Vec2 ARM_R_ANCHOR;

    EntityView(const uint16_t entityID, const EntityType type);

    const uint16_t getEntityID() const { return m_entityID; }
    const EntityType getEntityType() const { return m_type; }
    cocos2d::RefPtr<cocos2d::Sprite> getSprite() const { return m_sprite; }
    std::map<size_t, cocos2d::RefPtr<cocos2d::Sprite>> getSecondarySprites() const { return m_secondarySprites; }
    std::map<size_t, cocos2d::RefPtr<cocos2d::Action>>& getAnimations() { return m_animations; }

    void setupAnimations(CharacterType character);
    void removeAnimations();
    void runAnimation(const int animation);
    const size_t getAnimation(cocos2d::Action* action) const;
    
    void setupSprite(const std::string& frameName);
    void setupSecondarySprite(const std::string& frameName, const size_t index = 0);
    void removeSecondarySprite(const size_t index = 0);

    void setFlippedX(const bool flipX);
    
private:
    const uint16_t m_entityID;
    const EntityType m_type;
    cocos2d::RefPtr<cocos2d::Sprite> m_sprite;
    std::map<size_t, cocos2d::RefPtr<cocos2d::Sprite>> m_secondarySprites;
    std::map<size_t, cocos2d::RefPtr<cocos2d::Action>> m_animations;
    
    cocos2d::RefPtr<cocos2d::Sprite> createSprite(const std::string& frameName);

    cocos2d::RefPtr<cocos2d::Animation> createAnimation(const std::string frameName,
                                                        const size_t numFrames,
                                                        const float frameDelay);
};

#endif /* EntityView_h */
