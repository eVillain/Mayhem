#ifndef MasterServerMessages_h
#define MasterServerMessages_h

#include "Message.h"
#include "Address.h"
#include "Transport.h"
#include <vector>
#include <memory>

static const Net::MessageType PingMessageType = 0; // Everyone sends this occasionally to keep connections alive
static const Net::MessageType GameHostStatusUpdateMessageType = 1; // Hosts send this to update their status on the master list
static const Net::MessageType GameListRequestMessageType = 2; // Clients will send this to request a list of game hosts
static const Net::MessageType GameListResponseMessageType = 3; // Master Server will send this to clients including a list of hosts

static const Net::MessageType GameClientConnectionRequestMessageType = 4; // Clients will send this to request a link to a host
static const Net::MessageType GameClientConnectionResponseMessageType = 5; // Master server will send this to clients to confirm successful link
static const Net::MessageType GameHostConnectionResponseMessageType = 6; // Master server will send this to hosts to initiate a link with client

enum GameHostState {
    Disconnected = 0,
    Connecting,
    InLobby,
    InGame,
};

struct GameHostMasterData {
    Net::Address address;
    std::string name;
    GameHostState state;
    Net::Port gameClientPort;
    uint32_t totalCapacity;
    uint32_t currentPlayerCount;
    bool allowsJoining;

    float timeSinceUpdateReceived;
    float timeSinceUpdateSent;
    // list of clients?
};

class GameHostStatusUpdateMessage : public Net::Message
{
public:
    uint32_t totalCapacity;
    uint32_t currentPlayerCount;
    GameHostState state;
    Net::Port gameClientPort;
    bool allowsJoining;

    GameHostStatusUpdateMessage()
    : Message(GameHostStatusUpdateMessageType)
    , totalCapacity(0)
    , currentPlayerCount(0)
    , state(GameHostState::Disconnected)
    , gameClientPort(0)
    , allowsJoining(false)
    {}

    template <typename Stream> bool serializeInternal(Stream& stream)
    {
        stream.SerializeInteger(totalCapacity);
        stream.SerializeInteger(currentPlayerCount);
        stream.SerializeInteger(currentPlayerCount);
        stream.SerializeInteger((uint32_t&)state);
        stream.SerializeShort(gameClientPort);
        stream.SerializeBoolean(allowsJoining);
        return true;
    }
    
    bool serialize(Net::WriteStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::ReadStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::MeasureStream& stream) override { return serializeInternal(stream); }
};

class GameListRequestMessage : public Net::Message
{
public:
    uint32_t startRange;
    uint32_t endRange;
    
    GameListRequestMessage()
    : Message(GameListRequestMessageType)
    , startRange(0)
    , endRange(0)
    {}

    template <typename Stream> bool serializeInternal(Stream& stream)
    {
        stream.SerializeInteger(startRange);
        stream.SerializeInteger(endRange);
        return true;
    }
    
    bool serialize(Net::WriteStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::ReadStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::MeasureStream& stream) override { return serializeInternal(stream); }
};

class GameListResponseMessage : public Net::Message
{
public:
    uint32_t totalHosts;
    uint32_t startRange;
    uint32_t endRange;
    
    std::vector<Net::Transport::LobbyEntry> hosts;
    
    GameListResponseMessage()
    : Message(GameListResponseMessageType)
    , totalHosts(0)
    , startRange(0)
    , endRange(0)
    {}

    template <typename Stream> bool serializeInternal(Stream& stream)
    {
        stream.SerializeInteger(totalHosts);
        stream.SerializeInteger(startRange);
        stream.SerializeInteger(endRange);
        
        if (stream.IsReading())
        {
            for (uint32_t start = startRange; start < endRange; start++)
            {
                uint32_t addressInt = 0;
                Net::Port port = 0;
                std::string hostName;
                uint32_t maxClients = 0;
                uint32_t currentClients = 0;
                
                stream.SerializeInteger(addressInt);
                stream.SerializeShort(port);
                stream.SerializeString(hostName);
                stream.SerializeInteger(maxClients);
                stream.SerializeInteger(currentClients);

                Net::Transport::LobbyEntry data = {Net::Address(addressInt, port), hostName, maxClients, currentClients};
                hosts.push_back(data);
            }
        }
        else
        {
            for (Net::Transport::LobbyEntry& data : hosts)
            {
                uint32_t addressInt = data.address.GetAddress();
                Net::Port port = data.address.GetPort();
                stream.SerializeInteger(addressInt);
                stream.SerializeShort(port);
                stream.SerializeString(data.name);
                stream.SerializeInteger(data.maxClients);
                stream.SerializeInteger(data.currentClients);
            }
        }
        
        return true;
    }
    
    bool serialize(Net::WriteStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::ReadStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::MeasureStream& stream) override { return serializeInternal(stream); }
};

class GameClientConnectionRequestMessage : public Net::Message
{
public:
    Net::Address hostAddress;
    Net::Port clientPort;

    GameClientConnectionRequestMessage()
    : Message(GameClientConnectionRequestMessageType)
    , clientPort(0)
    {}

    template <typename Stream> bool serializeInternal(Stream& stream)
    {
        if (stream.IsReading())
        {
            uint32_t addressInt = 0;
            Net::Port port = 0;
            stream.SerializeInteger(addressInt);
            stream.SerializeShort(port);
            stream.SerializeShort(clientPort);
            hostAddress = Net::Address(addressInt, port);
        }
        else
        {
            uint32_t addressInt = hostAddress.GetAddress();
            Net::Port port = hostAddress.GetPort();
            stream.SerializeInteger(addressInt);
            stream.SerializeShort(port);
            stream.SerializeShort(clientPort);
        }
        return true;
    }
    
    bool serialize(Net::WriteStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::ReadStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::MeasureStream& stream) override { return serializeInternal(stream); }
};

class GameClientConnectionResponseMessage : public Net::Message
{
public:
    Net::Address hostAddress;
    bool success;

    GameClientConnectionResponseMessage()
    : Message(GameClientConnectionResponseMessageType)
    , success(false)
    {}

    template <typename Stream> bool serializeInternal(Stream& stream)
    {
        if (stream.IsReading())
        {
            uint32_t addressInt = 0;
            Net::Port port = 0;
            stream.SerializeInteger(addressInt);
            stream.SerializeShort(port);
            stream.SerializeBoolean(success);
            hostAddress = Net::Address(addressInt, port);
        }
        else
        {
            uint32_t addressInt = hostAddress.GetAddress();
            Net::Port port = hostAddress.GetPort();
            stream.SerializeInteger(addressInt);
            stream.SerializeShort(port);
            stream.SerializeBoolean(success);
        }
        return true;
    }
    
    bool serialize(Net::WriteStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::ReadStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::MeasureStream& stream) override { return serializeInternal(stream); }
};

class GameHostConnectionResponseMessage : public Net::Message
{
public:
    Net::Address clientAddress;
    bool success;

    GameHostConnectionResponseMessage()
    : Message(GameHostConnectionResponseMessageType)
    , success(false)
    {}

    template <typename Stream> bool serializeInternal(Stream& stream)
    {
        if (stream.IsReading())
        {
            uint32_t addressInt = 0;
            Net::Port port = 0;
            stream.SerializeInteger(addressInt);
            stream.SerializeShort(port);
            stream.SerializeBoolean(success);
            clientAddress = Net::Address(addressInt, port);
        }
        else
        {
            uint32_t addressInt = clientAddress.GetAddress();
            Net::Port port = clientAddress.GetPort();
            stream.SerializeInteger(addressInt);
            stream.SerializeShort(port);
            stream.SerializeBoolean(success);
        }
        return true;
    }
    
    bool serialize(Net::WriteStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::ReadStream& stream) override { return serializeInternal(stream); }
    bool serialize(Net::MeasureStream& stream) override { return serializeInternal(stream); }
};

#endif /* MasterServerMessages_h */
