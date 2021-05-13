#ifndef EntityInfoView_h
#define EntityInfoView_h

#include "cocos2d.h"
#include "cocos-ext.h"

class EntityInfoView : public cocos2d::Node
{
public:
    EntityInfoView();
    CREATE_FUNC(EntityInfoView);
    
    void setContentSize(const cocos2d::Size& size) override;
    
    const cocos2d::RefPtr<cocos2d::Label>& getNameLabel() const { return m_nameLabel; }
    const cocos2d::RefPtr<cocos2d::Label>& getPosLabel() const { return m_posLabel; }
    const cocos2d::RefPtr<cocos2d::Label>& getRotLabel() const { return m_rotLabel; }
    const cocos2d::RefPtr<cocos2d::Label>& getVelLabel() const { return m_velLabel; }
    const cocos2d::RefPtr<cocos2d::Label>& getAngVelLabel() const { return m_angVelLabel; }
    const cocos2d::RefPtr<cocos2d::Label>& getOwnerLabel() const { return m_ownerLabel; }

private:
    cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> m_background;
    cocos2d::RefPtr<cocos2d::ui::Scale9Sprite> m_border;
        
    cocos2d::RefPtr<cocos2d::Label> m_nameLabel;
    cocos2d::RefPtr<cocos2d::Label> m_posLabel;
    cocos2d::RefPtr<cocos2d::Label> m_rotLabel;
    cocos2d::RefPtr<cocos2d::Label> m_velLabel;
    cocos2d::RefPtr<cocos2d::Label> m_angVelLabel;
    cocos2d::RefPtr<cocos2d::Label> m_ownerLabel;
};

#endif /* EntityInfoView_h */
