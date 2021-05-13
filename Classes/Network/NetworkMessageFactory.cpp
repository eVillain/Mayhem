#include "NetworkMessageFactory.h"
#include "Network/NetworkMessages.h"

std::shared_ptr<Net::Message> NetworkMessageFactory::create(const Net::MessageType type)
{
    if (type == MESSAGE_TYPE_CLIENT_READY)
    {
        return std::make_shared<ClientReadyMessage>();
    }
    else if (type == MESSAGE_TYPE_CLIENT_STATE_UPDATE)
    {
        return std::make_shared<ClientStateUpdateMessage>();
    }
    else if (type == MESSAGE_TYPE_CLIENT_INPUT)
    {
        return std::make_shared<ClientInputMessage>();
    }
    else if (type == MESSAGE_TYPE_CLIENT_CHAT_MESSAGE)
    {
        return std::make_shared<ClientChatMessage>();
    }
    else if (type == MESSAGE_TYPE_SERVER_CHAT_MESSAGE)
    {
        return std::make_shared<ServerChatMessage>();
    }
    else if (type == MESSAGE_TYPE_SERVER_LOAD_LEVEL)
    {
        return std::make_shared<ServerLoadLevelMessage>();
    }
    else if (type == MESSAGE_TYPE_SERVER_STARTGAME)
    {
        return std::make_shared<ServerStartGameMessage>();
    }
    else if (type == MESSAGE_TYPE_SERVER_SNAPSHOT)
    {
        return std::make_shared<ServerSnapshotMessage>();
    }
    else if (type == MESSAGE_TYPE_SERVER_SNAPSHOT_DIFF)
    {
        return std::make_shared<ServerSnapshotDiffMessage>(m_dataCallback);
    }
    else if (type == MESSAGE_TYPE_SERVER_PLAYER_DEATH)
    {
        return std::make_shared<ServerPlayerDeathMessage>();
    }
    assert(false);

    return nullptr;
}

std::shared_ptr<Net::Message> NetworkMessageFactory::create(Net::ReadStream& stream)
{
    Net::MessageType type = MESSAGE_TYPES_COUNT;
    stream.SerializeByte(type);
    
    std::shared_ptr<Net::Message> message = create(type);
    
    if (message)
    {
        message->serialize(stream);
    }
    return message;
}

uint32_t NetworkMessageFactory::getNumTypes() const
{
    return MESSAGE_TYPES_COUNT;
}

void NetworkMessageFactory::setDeltaDataCallback(std::function<const SnapshotData&(const uint32_t)> dataCallback)
{
    m_dataCallback = dataCallback;
}

const std::string NetworkMessageFactory::getNameForType(const Net::MessageType type)
{
    if (type == MESSAGE_TYPE_CLIENT_READY)
    {
        return "Player Ready";
    }
    else if (type == MESSAGE_TYPE_CLIENT_STATE_UPDATE)
    {
        return "Client State Update";
    }
    else if (type == MESSAGE_TYPE_CLIENT_INPUT)
    {
        return "Player Input";
    }
    else if (type == MESSAGE_TYPE_CLIENT_CHAT_MESSAGE)
    {
        return "Player Chat Message";
    }
    else if (type == MESSAGE_TYPE_SERVER_CHAT_MESSAGE)
    {
        return "Server Chat Message";
    }
    else if (type == MESSAGE_TYPE_SERVER_LOAD_LEVEL)
    {
        return "Server Load Level";
    }
    else if (type == MESSAGE_TYPE_SERVER_STARTGAME)
    {
        return "Server Start Game";
    }
    else if (type == MESSAGE_TYPE_SERVER_SNAPSHOT)
    {
        return "Server Snapshot";
    }
    else if (type == MESSAGE_TYPE_SERVER_PLAYER_DEATH)
    {
        return "Server Player Death";
    }
    return "Unknown Message Type";
}
