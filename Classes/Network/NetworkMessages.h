#ifndef NetworkMessages_h
#define NetworkMessages_h

#include "Game/Shared/EntityConstants.h"

#include "Network/DrudgeNet/include/Message.h"
#include "cocos2d.h"

enum MessageTypes {
    MESSAGE_TYPE_CLIENT_READY = 0,
    MESSAGE_TYPE_CLIENT_STATE_UPDATE,
    MESSAGE_TYPE_CLIENT_INPUT,
    MESSAGE_TYPE_CLIENT_CHAT_MESSAGE,
    MESSAGE_TYPE_SERVER_CHAT_MESSAGE,
    MESSAGE_TYPE_SERVER_LOAD_LEVEL,
    MESSAGE_TYPE_SERVER_STARTGAME,
    MESSAGE_TYPE_SERVER_SNAPSHOT,
    MESSAGE_TYPE_SERVER_SNAPSHOT_DIFF,
    MESSAGE_TYPE_SERVER_PLAYER_DEATH,
    MESSAGE_TYPE_SERVER_TILE_DEATH,
    MESSAGE_TYPES_COUNT
};

enum ClientState {
    PLAYER_NOT_READY = 0,
    PLAYER_READY,
    LEVEL_LOADED,
    GAME_STARTED,
    PLAYER_RESPAWN,
};

class ClientReadyMessage : public Net::Message {
public:
    // TODO: This should eventually contain player customization data
    bool ready;
    
    ClientReadyMessage()
    : Message(MESSAGE_TYPE_CLIENT_READY) {}

    template <typename Stream> bool serializeInternal(Stream& stream)
    {
        stream.SerializeBoolean(ready);
        return true;
    }
    
    bool serialize(Net::WriteStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::ReadStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::MeasureStream& stream) override { return serializeInternal(stream); }
};

class ClientStateUpdateMessage : public Net::Message {
public:
    // TODO: This should eventually contain player customization data
    uint8_t state;
    
    ClientStateUpdateMessage()
    : Message(MESSAGE_TYPE_CLIENT_STATE_UPDATE) {}

    template <typename Stream> bool serializeInternal(Stream& stream)
    {
        stream.SerializeByte(state);
        return true;
    }
    
    bool serialize(Net::WriteStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::ReadStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::MeasureStream& stream) override { return serializeInternal(stream); }
};

class ServerLoadLevelMessage : public Net::Message {
public:
    uint32_t tickRate;
    uint8_t modeType;
    uint8_t maxPlayers;
    uint8_t playersPerTeam;
    std::string level;
    
    ServerLoadLevelMessage()
    : Message(MESSAGE_TYPE_SERVER_LOAD_LEVEL) {}

    template <typename Stream> bool serializeInternal(Stream& stream)
    {
        stream.SerializeInteger(tickRate);
        stream.SerializeByte(modeType);
        stream.SerializeByte(maxPlayers);
        stream.SerializeByte(playersPerTeam);
        stream.SerializeString(level);

        return true;
    }
    
    bool serialize(Net::WriteStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::ReadStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::MeasureStream& stream) override { return serializeInternal(stream); }
};

class ServerStartGameMessage : public Net::Message {
public:
    uint8_t playerID;
    
    ServerStartGameMessage()
    : Message(MESSAGE_TYPE_SERVER_STARTGAME) {}

    template <typename Stream> bool serializeInternal(Stream& stream)
    {
        stream.SerializeByte(playerID);
        return true;
    }
    
    bool serialize(Net::WriteStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::ReadStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::MeasureStream& stream) override { return serializeInternal(stream); }
};

class ClientChatMessage : public Net::Message {
public:
    std::string text;
    
    ClientChatMessage()
    : Message(MESSAGE_TYPE_CLIENT_CHAT_MESSAGE) {}

    template <typename Stream> bool serializeInternal(Stream& stream)
    {
         if (!stream.SerializeString(text))
         {
             return false;
         }

        return true;
    }
    
    bool serialize(Net::WriteStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::ReadStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::MeasureStream& stream) override { return serializeInternal(stream); }
};

class ServerChatMessage : public Net::Message {
public:
    uint8_t playerID;
    std::string text;
    
    ServerChatMessage()
    : Message(MESSAGE_TYPE_SERVER_CHAT_MESSAGE) {}

    template <typename Stream> bool serializeInternal(Stream& stream)
    {
        if (!stream.SerializeByte(playerID))
        {
            return false;
        }
        if (!stream.SerializeString(text))
        {
            return false;
        }

        return true;
    }
    
    bool serialize(Net::WriteStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::ReadStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::MeasureStream& stream) override { return serializeInternal(stream); }
};

class ClientInputMessage : public Net::Message {
public:
    uint32_t inputSequence;
    uint32_t lastReceivedSnapshot;
    float directionX;
    float directionY;
    float aimPointX;
    float aimPointY;
    bool jump;
    bool shoot;
    bool interact;
    bool run;
    bool reload;
    bool changeWeapon;
    uint8_t slot;

    uint8_t pickUpType;
    uint16_t pickUpAmount;
    uint16_t pickUpID;

    ClientInputMessage()
    : Message(MESSAGE_TYPE_CLIENT_INPUT) {}
    
    ClientInputMessage(const ClientInputMessage& other)
    : Message(MESSAGE_TYPE_CLIENT_INPUT)
    , inputSequence(other.inputSequence)
    , lastReceivedSnapshot(other.lastReceivedSnapshot)
    , directionX(other.directionX)
    , directionY(other.directionY)
    , aimPointX(other.aimPointX)
    , aimPointY(other.aimPointY)
    , jump(other.jump)
    , shoot(other.shoot)
    , interact(other.interact)
    , run(other.run)
    , reload(other.reload)
    , changeWeapon(other.changeWeapon)
    , slot(other.slot)
    , pickUpType(other.pickUpType)
    , pickUpAmount(other.pickUpAmount)
    , pickUpID(other.pickUpID)
    {}
    
    template <typename Stream> bool serializeInternal(Stream& stream)
    {
        stream.SerializeBits(inputSequence, 32);
        stream.SerializeBits(lastReceivedSnapshot, 32);
        stream.SerializeFloat(directionX);
        stream.SerializeFloat(directionY);
        stream.SerializeFloat(aimPointX);
        stream.SerializeFloat(aimPointY);
        stream.SerializeBoolean(jump);
        stream.SerializeBoolean(shoot);
        stream.SerializeBoolean(interact);
        stream.SerializeBoolean(run);
        stream.SerializeBoolean(reload);
        stream.SerializeBoolean(changeWeapon);
        stream.SerializeByte(slot);
        
        if (stream.IsReading())
        {
//            bool hasPickup = false;
//            stream.SerializeBoolean(hasPickup);
//            if (hasPickup)
            {
                uint32_t amount;
                uint32_t entityID;
                stream.SerializeByte(pickUpType);
                stream.SerializeBits(amount, 16);
                stream.SerializeBits(entityID, 16);
                pickUpAmount = amount;
                pickUpID = entityID;
            }
        }
        else
        {
//            bool hasPickup = pickUpType > 0;
//            stream.SerializeBoolean(hasPickup);
//            if (hasPickup)
            {
                uint32_t amount = pickUpAmount;
                uint32_t entityID = pickUpID;
                stream.SerializeByte(pickUpType);
                stream.SerializeBits(amount, 16);
                stream.SerializeBits(entityID, 16);
            }
        }
        return true;
    }
    
    bool serialize(Net::WriteStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::ReadStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::MeasureStream& stream) override { return serializeInternal(stream); }
};

struct EntitySnapshot {
    float positionX;
    float positionY;
    float rotation;
    float velocityX;
    float velocityY;
    float angularVelocity;
    uint8_t type;
    uint16_t amount;
    uint8_t ownerID;
};

struct InventoryItemState {
    uint8_t type;
    uint16_t amount;
};

struct PlayerState {
    uint32_t entityID;
    uint8_t animationState;
    float aimPointX;
    float aimPointY;
    float health;
    bool flipX;
    bool weaponFired;
    uint8_t activeWeaponSlot;
    std::vector<InventoryItemState> weaponSlots;
};

struct FrameHitData {
    uint16_t hitterEntityID;
    uint16_t hitEntityID;
    float damage;
    float hitPosX;
    float hitPosY;
    bool headShot;
};

struct SnapshotData {
    uint32_t serverTick;
    uint32_t lastReceivedInput;
    uint8_t playerCount;
    uint32_t entityCount;
    std::map<uint8_t, PlayerState> playerData;
    std::map<uint32_t, EntitySnapshot> entityData;
    std::vector<InventoryItemState> inventory;
    std::vector<FrameHitData> hitData;
};

struct SnapshotDiffData {
    uint32_t serverTick;
    uint32_t lastReceivedInput;
    uint32_t entityCount;
    uint8_t playerCount;

    std::map<uint8_t, PlayerState> playerData;
    std::map<uint32_t, EntitySnapshot> entityData;
    std::vector<InventoryItemState> inventory;
    std::vector<FrameHitData> hitData;
};

static const SnapshotData SNAPSHOT_ZERO = {0,0,0,0};

class ServerSnapshotMessage : public Net::Message {
public:
    SnapshotData data;
    
    ServerSnapshotMessage()
    : Message(MESSAGE_TYPE_SERVER_SNAPSHOT) {}
    
    template <typename Stream> bool serializeInternal(Stream& stream)
    {
        stream.SerializeBits(data.serverTick, 32);
        stream.SerializeBits(data.lastReceivedInput, 32);
        if (stream.IsReading())
        {
            stream.SerializeByte(data.playerCount);
            for (size_t i = 0; i < data.playerCount; i++)
            {
                uint8_t playerID = 0;
                uint32_t entityID = 0;
                uint8_t animationState = 0;
                float aimPointX = 0.f;
                float aimPointY = 0.f;
                bool flipX = false;
                bool weaponFired = false;
                float health = 0.f;
                uint8_t activeWeaponSlot = 0;
                std::vector<InventoryItemState> weaponSlots;

                stream.SerializeByte(playerID);
                stream.SerializeBits(entityID, 16);
                stream.SerializeByte(animationState);
                stream.SerializeFloat(aimPointX);
                stream.SerializeFloat(aimPointY);
                stream.SerializeFloat(health);
                stream.SerializeBoolean(flipX);
                stream.SerializeBoolean(weaponFired);
                stream.SerializeByte(activeWeaponSlot);
                
                for (size_t i = 0; i < 5; i++)
                {
                    uint8_t type;
                    uint16_t amount;
                    stream.SerializeByte(type);
                    stream.SerializeShort(amount);
                    weaponSlots.push_back({type, amount});
                }
                
                if (!stream.IsMeasuring())
                {
                    data.playerData[playerID] = { entityID, animationState, aimPointX, aimPointY, health, flipX, weaponFired, activeWeaponSlot, weaponSlots };
                }
            }
            
            stream.SerializeBits(data.entityCount, 32);
            for (size_t i = 0; i < data.entityCount; i++)
            {
                uint32_t entityID = 0;
                float positionX;
                float positionY;
                float rotation;
                float velocityX;
                float velocityY;
                float angularVelocity;
                uint8_t entityType = 0;
                stream.SerializeBits(entityID, 16);
                stream.SerializeFloat(positionX);
                stream.SerializeFloat(positionY);
                stream.SerializeFloat(rotation);
                stream.SerializeFloat(velocityX);
                stream.SerializeFloat(velocityY);
                stream.SerializeFloat(angularVelocity);
                stream.SerializeByte(entityType);
                if (!stream.IsMeasuring())
                {
                    data.entityData[entityID] = { positionX, positionY, rotation, velocityX, velocityY, angularVelocity, entityType, 0, 0 };
                }
            }
            
            uint32_t inventoryCount = 0;
            stream.SerializeBits(inventoryCount, 32);
            for (size_t i = 0; i < inventoryCount; i++)
            {
                uint8_t entityType;
                uint8_t amount;
                stream.SerializeByte(entityType);
                stream.SerializeByte(amount);
                if (!stream.IsMeasuring())
                {
                    data.inventory.push_back({entityType, amount});
                }
            }
            uint32_t hitCount = 0;
            stream.SerializeBits(hitCount, 32);
            for (size_t i = 0; i < hitCount; i++)
            {
                FrameHitData hitData;
                uint32_t hitterEntityID = 0;
                uint32_t hitEntityID = 0;
                stream.SerializeBits(hitterEntityID, 16);
                stream.SerializeBits(hitEntityID, 16);
                hitData.hitterEntityID = hitterEntityID;
                hitData.hitEntityID = hitEntityID;
                stream.SerializeFloat(hitData.damage);
                stream.SerializeFloat(hitData.hitPosX);
                stream.SerializeFloat(hitData.hitPosY);
                if (!stream.IsMeasuring())
                {
                    data.hitData.push_back(hitData);
                }
            }
        }
        else
        {
            data.playerCount = (uint8_t)data.playerData.size();
            stream.SerializeByte(data.playerCount);
            for (auto pair : data.playerData)
            {
                uint8_t playerID = pair.first;
                stream.SerializeByte(playerID);
                stream.SerializeBits(pair.second.entityID, 16);
                stream.SerializeByte(pair.second.animationState);
                stream.SerializeFloat(pair.second.aimPointX);
                stream.SerializeFloat(pair.second.aimPointY);
                stream.SerializeFloat(pair.second.health);
                stream.SerializeBoolean(pair.second.flipX);
                stream.SerializeBoolean(pair.second.weaponFired);
                stream.SerializeByte(pair.second.activeWeaponSlot);
                for (size_t i = 0; i < 5; i++)
                {
                    stream.SerializeByte(pair.second.weaponSlots.at(i).type);
                    stream.SerializeShort(pair.second.weaponSlots.at(i).amount);
                }
            }
            
            data.entityCount = (uint32_t)data.entityData.size();
            stream.SerializeBits(data.entityCount, 32);
            for (auto pair : data.entityData)
            {
                uint32_t entityID = pair.first;
                stream.SerializeBits(entityID, 16);
                stream.SerializeFloat(pair.second.positionX);
                stream.SerializeFloat(pair.second.positionY);
                stream.SerializeFloat(pair.second.rotation);
                stream.SerializeFloat(pair.second.velocityX);
                stream.SerializeFloat(pair.second.velocityY);
                stream.SerializeFloat(pair.second.angularVelocity);
                stream.SerializeByte(pair.second.type);
            }
            
            uint32_t inventoryCount = (uint32_t)data.inventory.size();
            stream.SerializeBits(inventoryCount, 32);
            for (const auto& inventoryItem : data.inventory)
            {
                uint8_t entityType = inventoryItem.type;
                uint8_t amount = inventoryItem.amount;
                stream.SerializeByte(entityType);
                stream.SerializeByte(amount);
            }
            uint32_t hitCount = (uint32_t)data.hitData.size();
            stream.SerializeBits(hitCount, 32);
            for (const auto& hit : data.hitData)
            {
                uint32_t hitPlayerID = hit.hitEntityID;
                uint32_t hitterPlayerID = hit.hitterEntityID;
                float damage = hit.damage;
                float hitPosX = hit.hitPosX;
                float hitPosY = hit.hitPosY;
                stream.SerializeBits(hitterPlayerID, 16);
                stream.SerializeBits(hitPlayerID, 16);
                stream.SerializeFloat(damage);
                stream.SerializeFloat(hitPosX);
                stream.SerializeFloat(hitPosY);
            }
        }
        return true;
    }
    
    bool serialize(Net::WriteStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::ReadStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::MeasureStream& stream) override { return serializeInternal(stream); }
};

class ServerSnapshotDiffMessage : public Net::Message {
public:
    SnapshotData data;
    uint32_t previousServerTick;

    ServerSnapshotDiffMessage(std::function<const SnapshotData&(const uint32_t)> getDataCallback)
    : Message(MESSAGE_TYPE_SERVER_SNAPSHOT_DIFF)
    , m_getDataCallback(getDataCallback) {}
    
    template <typename Stream> bool streamBitsDiff(Stream& stream,
                                                   uint32_t previous,
                                                   uint32_t& value,
                                                   int32_t bits)
    {
        if (stream.IsReading())
        {
            bool valueChanged = false;
            stream.SerializeBoolean(valueChanged);
            if (valueChanged)
            {
                return stream.SerializeBits(value, bits);
            }
            else
            {
                value = previous;
            }
        }
        else
        {
            bool valueChanged = previous != value;
            stream.SerializeBoolean(valueChanged);
            if (valueChanged)
            {
                return stream.SerializeBits(value, bits);
            }
        }
        return true;
    }
    
    template <typename Stream> bool streamByteDiff(Stream& stream,
                                                   uint8_t previous,
                                                   uint8_t& value)
    {
        if (stream.IsReading())
        {
            bool valueChanged = false;
            stream.SerializeBoolean(valueChanged);
            if (valueChanged)
            {
                return stream.SerializeByte(value);
            }
            else
            {
                value = previous;
            }
        }
        else
        {
            bool valueChanged = previous != value;
            stream.SerializeBoolean(valueChanged);
            if (valueChanged)
            {
                return stream.SerializeByte(value);
            }
        }
        return true;
    }
    
    template <typename Stream> bool streamShortDiff(Stream& stream,
                                                    uint16_t previous,
                                                    uint16_t& value)
    {
        if (stream.IsReading())
        {
            bool valueChanged = false;
            stream.SerializeBoolean(valueChanged);
            if (valueChanged)
            {
                return stream.SerializeShort(value);
            }
            else
            {
                value = previous;
            }
        }
        else
        {
            bool valueChanged = previous != value;
            stream.SerializeBoolean(valueChanged);
            if (valueChanged)
            {
                return stream.SerializeShort(value);
            }
        }
        return true;
    }
    
    template <typename Stream> bool streamFloatDiff(Stream& stream,
                                                    float previous,
                                                    float& value)
    {
        if (stream.IsReading())
        {
            bool valueChanged = false;
            stream.SerializeBoolean(valueChanged);
            if (valueChanged)
            {
                return stream.SerializeFloat(value);
            }
            else
            {
                value = previous;
            }
        }
        else
        {
            bool valueChanged = previous != value;
            stream.SerializeBoolean(valueChanged);
            if (valueChanged)
            {
                return stream.SerializeFloat(value);
            }
        }
        return true;
    }
    
    template <typename Stream> bool serializeInternal(Stream& stream)
    {
        stream.SerializeBits(data.serverTick, 32);
        stream.SerializeBits(data.lastReceivedInput, 32);
        stream.SerializeBits(previousServerTick, 32);

        if (!m_getDataCallback)
        {
            return false;
        }
        
        // At this point the previous tick is known so we can load the data for that
        const SnapshotData& previousState = m_getDataCallback(previousServerTick);
        
        if (stream.IsReading())
        {
            streamByteDiff(stream, previousState.playerCount, data.playerCount);

            for (size_t i = 0; i < data.playerCount; i++)
            {
                uint8_t playerID = 0;
                uint32_t entityID = 0;
                stream.SerializeByte(playerID);
                stream.SerializeBits(entityID, 16);
                
                uint8_t animationState = 0;
                float aimPointX = 0.f;
                float aimPointY = 0.f;
                bool flipX = false;
                bool weaponFired = false;
                float health = 0.f;
                uint8_t activeWeaponSlot = 0;
                std::vector<InventoryItemState> weaponSlots;
                
                auto previousPlayerIt = previousState.playerData.find(playerID);
                if (previousPlayerIt != previousState.playerData.end())
                {
                    streamByteDiff(stream, previousPlayerIt->second.animationState, animationState);
                    streamFloatDiff(stream, previousPlayerIt->second.aimPointX, aimPointX);
                    streamFloatDiff(stream, previousPlayerIt->second.aimPointY, aimPointY);
                    streamFloatDiff(stream, previousPlayerIt->second.health, health);
                }
                else
                {
                    streamByteDiff(stream, 0, animationState);
                    streamFloatDiff(stream, 0.f, aimPointX);
                    streamFloatDiff(stream, 0.f, aimPointY);
                    streamFloatDiff(stream, 0.f, health);
                }
                stream.SerializeBoolean(flipX);
                stream.SerializeBoolean(weaponFired);
                
                if (previousPlayerIt != previousState.playerData.end())
                {
                    streamByteDiff(stream, previousPlayerIt->second.activeWeaponSlot, activeWeaponSlot);
                }
                else
                {
                    streamByteDiff(stream, 0, activeWeaponSlot);
                }
                
                for (size_t i = 0; i < 5; i++)
                {
                    uint8_t type;
                    uint16_t amount;
                    
                    if (previousPlayerIt != previousState.playerData.end())
                    {
                        streamByteDiff(stream, previousPlayerIt->second.weaponSlots.at(i).type, type);
                        streamShortDiff(stream, previousPlayerIt->second.weaponSlots.at(i).amount, amount);
                    }
                    else
                    {
                        streamByteDiff(stream, 0, type);
                        streamShortDiff(stream, 0, amount);
                    }
                    
                    stream.SerializeByte(type);
                    stream.SerializeShort(amount);
                    weaponSlots.push_back({type, amount});
                }
                
                if (!stream.IsMeasuring())
                {
                    data.playerData[playerID] = { entityID, animationState, aimPointX, aimPointY, health, flipX, weaponFired, activeWeaponSlot, weaponSlots };
                }
            }
        }
        else
        {
            data.playerCount = (uint8_t)data.playerData.size();
            streamByteDiff(stream, previousState.playerCount, data.playerCount);

            for (auto pair : data.playerData)
            {
                uint8_t playerID = pair.first;
                stream.SerializeByte(playerID);
                stream.SerializeBits(pair.second.entityID, 16);
                
                auto previousPlayerIt = previousState.playerData.find(playerID);
                if (previousPlayerIt != previousState.playerData.end())
                {
                    streamByteDiff(stream, previousPlayerIt->second.animationState, pair.second.animationState);
                    streamFloatDiff(stream, previousPlayerIt->second.aimPointX, pair.second.aimPointX);
                    streamFloatDiff(stream, previousPlayerIt->second.aimPointY, pair.second.aimPointY);
                    streamFloatDiff(stream, previousPlayerIt->second.health, pair.second.health);
                }
                else
                {
                    streamByteDiff(stream, 0, pair.second.animationState);
                    streamFloatDiff(stream, 0.f, pair.second.aimPointX);
                    streamFloatDiff(stream, 0.f, pair.second.aimPointY);
                    streamFloatDiff(stream, 0, pair.second.health);
                }
                
                stream.SerializeBoolean(pair.second.flipX);
                stream.SerializeBoolean(pair.second.weaponFired);
                
                if (previousPlayerIt != previousState.playerData.end())
                {
                    streamByteDiff(stream, previousPlayerIt->second.activeWeaponSlot, pair.second.activeWeaponSlot);
                }
                else
                {
                    streamByteDiff(stream, 0, pair.second.activeWeaponSlot);
                }
                
                for (size_t i = 0; i < 5; i++)
                {
                    if (previousPlayerIt != previousState.playerData.end())
                    {
                        streamByteDiff(stream, previousPlayerIt->second.weaponSlots.at(i).type, pair.second.weaponSlots.at(i).type);
                        streamShortDiff(stream, previousPlayerIt->second.weaponSlots.at(i).amount, pair.second.weaponSlots.at(i).amount);
                    }
                    else
                    {
                        streamByteDiff(stream, 0, pair.second.weaponSlots.at(i).type);
                        streamShortDiff(stream, 0, pair.second.weaponSlots.at(i).amount);
                    }
                }
            }
        }
        return true;
    }
    
    bool serialize(Net::WriteStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::ReadStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::MeasureStream& stream) override { return serializeInternal(stream); }
    
private:
    std::function<const SnapshotData&(const uint32_t)> m_getDataCallback;
};

class ServerPlayerDeathMessage : public Net::Message {
public:
    uint8_t deadPlayerID;
    uint32_t killerEntityID;
    EntityType killerType;
    bool headshot;

    ServerPlayerDeathMessage()
    : Message(MESSAGE_TYPE_SERVER_PLAYER_DEATH) {}

    template <typename Stream> bool serializeInternal(Stream& stream)
    {
        stream.SerializeByte(deadPlayerID);
        stream.SerializeBits(killerEntityID, 16);
        if (stream.IsReading())
        {
            uint8_t entityType = 0;
            stream.SerializeByte(entityType);
            killerType = (EntityType)entityType;
        }
        else
        {
            uint8_t entityType = killerType;
            stream.SerializeByte(entityType);
        }
        
        stream.SerializeBoolean(headshot);
        
        return true;
    }
    
    bool serialize(Net::WriteStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::ReadStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::MeasureStream& stream) override { return serializeInternal(stream); }
};

class ServerTileDeathMessage : public Net::Message {
public:
    uint32_t tileX;
    uint32_t tileY;

    ServerTileDeathMessage()
    : Message(MESSAGE_TYPE_SERVER_TILE_DEATH) {}

    template <typename Stream> bool serializeInternal(Stream& stream)
    {
        if (stream.IsReading())
        {
            tileX = 0;
            tileY = 0;
        }
        stream.SerializeInteger(tileX);
        stream.SerializeInteger(tileY);
        
        return true;
    }
    
    bool serialize(Net::WriteStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::ReadStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::MeasureStream& stream) override { return serializeInternal(stream); }
};


#endif /* NetworkMessages_h */
