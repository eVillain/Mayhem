#ifndef PlayerLogic_h
#define PlayerLogic_h

#include "Network/NetworkMessages.h"
#include "Game/Shared/EntityConstants.h"


class PlayerLogic
{
public:
    static void applyInput(const uint8_t playerID,
                           SnapshotData& snapshot,
                           std::shared_ptr<ClientInputMessage>& input,
                           float& lastActionTime,
                           const float currentTime);
    static bool checkForReload(SnapshotData& snapshot,
                               PlayerState& player,
                               EntitySnapshot& entity,
                               const std::shared_ptr<ClientInputMessage>& input,
                               float& lastActionTime,
                               const float currentTime);
  
    static bool isReloading(const AnimationState animationState,
                            const float lastActionTime,
                            const float currentTime);
    
    static bool canReload(const SnapshotData& snapshot,
                          const PlayerState& player,
                          const float lastActionTime,
                          const float currentTime);
    
    static bool getEntityAtPoint(uint32_t& outEntityID,
                                 const SnapshotData& snapshot,
                                 const cocos2d::Vec2& point,
                                 const uint32_t ignoreEntityID);
    
    static uint16_t getInventoryAmount(const EntityType type,
                                       const std::vector<InventoryItemState>& inventory);
    
    static void setInventoryAmount(const EntityType type,
                                   const uint16_t amount,
                                   std::vector<InventoryItemState>& inventory);
};

#endif /* PlayerLogic_h */
