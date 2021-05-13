#ifndef Item_h
#define Item_h

#include "Entity.h"

class Item : public Entity
{
public:
    Item(const uint16_t entityID,
         const EntityType& type);
    ~Item();
    
    void setOwnerID(uint16_t ownerID) { m_ownerID = ownerID; }
    uint16_t getOwnerID() const { return m_ownerID; }

    void setAmount(uint16_t amount) { m_amount = amount; }
    uint16_t getAmount() const { return m_amount; }

protected:
    uint16_t m_ownerID;
    uint16_t m_amount;
};


#endif /* Item_h */
