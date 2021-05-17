#include "SnapshotModel.h"

#include "Entity.h"
#include "Player.h"
#include "SharedConstants.h"

SnapshotModel::SnapshotModel()
: m_lastReceivedSnapshot(0)
, m_lastAppliedSnapshot(0)
{
    printf("SnapshotModel:: constructor: %p\n", this);
}

SnapshotModel::~SnapshotModel()
{
    printf("SnapshotModel:: destructor: %p\n", this);
}

void SnapshotModel::reset()
{
    m_lastReceivedSnapshot = 0;
    m_lastAppliedSnapshot = 0;
    m_snapshots.clear();
}

bool SnapshotModel::storeSnapshot(const SnapshotData& data)
{
    const bool isOlderThanLastSnapshot = m_lastReceivedSnapshot >= data.serverTick;
    const bool isFirstSnapshot = (m_lastReceivedSnapshot == 0 && data.serverTick == 0 && m_snapshots.size() == 0);
    if (isOlderThanLastSnapshot && !isFirstSnapshot)
    {
        CCLOG("SnapshotModel::storeSnapshot fail - snapshot tick %u older than last received: %u\n",
              data.serverTick, m_snapshots.back().serverTick);
        return false;
    }
//    CCLOG("SnapshotModel::storeSnapshot snapshot for tick %u received", data.serverTick);

    m_lastReceivedSnapshot = data.serverTick;
    m_snapshots.push_back(data);
    return true;
}

SnapshotData SnapshotModel::interpolateSnapshots(const SnapshotData& from,
                                                 const SnapshotData& to,
                                                 const float alpha)
{
    SnapshotData interpolatedSnapshot;
    interpolatedSnapshot.serverTick = to.serverTick;
    interpolatedSnapshot.lastReceivedInput = to.lastReceivedInput;
    interpolatedSnapshot.playerCount = to.playerCount;
    interpolatedSnapshot.entityCount = to.entityCount;
    
    for (const auto& pair : from.entityData)
    {
        const uint32_t entityID = pair.first;
        if (!to.entityData.count(entityID))
        {
            continue;
        }
        
        const auto& toData = to.entityData.at(entityID);
        const auto& fromData = pair.second;
        interpolatedSnapshot.entityData[entityID] = interpolateEntitySnapshot(fromData, toData, alpha);
    }
    
    const auto& fromPlayerData = from.playerData;
    const auto& toPlayerData = to.playerData;
    for (const auto& pair : fromPlayerData)
    {
        const uint8_t playerID = pair.first;
        const auto toPlayerIt = toPlayerData.find(playerID);
        if (toPlayerIt == toPlayerData.end())
        {
            continue;
        }
        
        const auto& toData = toPlayerIt->second;
        const auto& fromData = pair.second;
        interpolatedSnapshot.playerData[playerID] = interpolatePlayerState(fromData, toData, alpha);
    }
    
    return interpolatedSnapshot;
}

EntitySnapshot SnapshotModel::interpolateEntitySnapshot(const EntitySnapshot& from,
                                                        const EntitySnapshot& to,
                                                        const float alpha)
{
    EntitySnapshot interpolated;
    
    const cocos2d::Vec2 fromPosition = cocos2d::Vec2(from.positionX, from.positionY);
    const cocos2d::Vec2 toPosition = cocos2d::Vec2(to.positionX,  to.positionY);
    const cocos2d::Vec2 position = fromPosition.lerp(toPosition, alpha);
    interpolated.positionX = position.x;
    interpolated.positionY = position.y;
    interpolated.rotation = (from.rotation * (1.0f - alpha)) + (to.rotation * alpha);
    
    return interpolated;
}

PlayerState SnapshotModel::interpolatePlayerState(const PlayerState& from,
                                                  const PlayerState& to,
                                                  const float alpha)
{
    PlayerState interpolated;
    
    const cocos2d::Vec2 toAimPosition = cocos2d::Vec2(to.aimPointX, to.aimPointY);
    const cocos2d::Vec2 fromAimPosition = cocos2d::Vec2(from.aimPointX, from.aimPointY);
    const cocos2d::Vec2 aimPosition = fromAimPosition.lerp(toAimPosition, alpha);
    
    interpolated.entityID = from.entityID;
    interpolated.animationState = from.animationState;
    interpolated.activeWeaponSlot = from.activeWeaponSlot;
    interpolated.weaponSlots = from.weaponSlots;
    interpolated.aimPointX = aimPosition.x;
    interpolated.aimPointY = aimPosition.y;
    interpolated.health = (from.health * (1.0f - alpha)) + (to.health * alpha);
    interpolated.flipX = from.flipX;
    
    return interpolated;
}

size_t SnapshotModel::getSnapshotIndexForFrame(const uint32_t frame) const
{
    for (size_t index = 0; index < m_snapshots.size(); index++)
    {
        if (m_snapshots.at(index).serverTick == frame)
        {
            return index;
        }
    }
    
    return 0;
}

void SnapshotModel::eraseUpToIndex(const size_t index)
{
    m_snapshots.erase(m_snapshots.begin(), m_snapshots.begin() + index);
}
