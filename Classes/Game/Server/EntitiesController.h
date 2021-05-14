#ifndef EntitiesController_h
#define EntitiesController_h

#include "Game/Shared/EntityConstants.h"


class Entity;
class EntitiesModel;
class Event;
class Player;
class Item;
class Projectile;

class EntitiesController
{
public:
    EntitiesController(std::shared_ptr<EntitiesModel> entitiesModel);
    ~EntitiesController();
    
    void initialize();
    
    void update(const float deltaTime);
    
    std::shared_ptr<Player> createPlayer(const uint8_t playerID,
                                         const uint16_t entityID,
                                         const cocos2d::Vec2& position,
                                         const float rotation);
    std::shared_ptr<Item> createItem(const uint16_t entityID,
                                     const EntityType type,
                                     const cocos2d::Vec2& position,
                                     const float rotation);
    
    std::shared_ptr<Projectile> createProjectile(const uint16_t entityID,
                                                 const EntityType type,
                                                 const cocos2d::Vec2& position,
                                                 const cocos2d::Vec2& velocity,
                                                 const float rotation);
    
    void destroyEntity(const uint16_t entityID);

private:
    void removeDeadEntities();
    
    std::shared_ptr<EntitiesModel> m_model;
    
    std::vector<uint16_t> m_deadEntities;
};


#endif /* EntitiesController_h */
