#ifndef FakeNet_h
#define FakeNet_h

#include <queue>
#include <functional>
#include "Network/NetworkMessages.h"
#include "ReliabilitySystem.h"

class FakeNet
{
public:
    FakeNet();
    ~FakeNet();

    void terminate();
    
    void update(const float deltaTime);
    
    void takeClientData(const uint8_t playerID, const unsigned char* data, const size_t dataSize);
    void takeServerData(const unsigned char* data, const size_t dataSize);
    
    void setClientDataCallback(std::function<void(const uint8_t playerID, const unsigned char* data, const size_t dataSize)> cb);
    void setServerDataCallback(std::function<void(const unsigned char* data, const size_t dataSize)> cb);
    
    void setInputDelay(const float delay) { m_inputDelay = delay; }
    void setServerDelay(const float delay) { m_serverDelay = delay; }
    
    float getInputDelay() const { return m_inputDelay; }
    float getServerDelay() const { return m_serverDelay; }

private:
    struct ClientData {
        float time;
        uint8_t playerID;
        const unsigned char* data;
        const size_t dataSize;
    };
    struct ServerData {
        float time;
        const unsigned char* data;
        const size_t dataSize;
    };
    
    std::queue<ClientData> m_clientData;
    std::queue<ServerData> m_serverData;

    std::function<void(const uint8_t playerID, const unsigned char* data, const size_t dataSize)> m_clientDataCallback;
    std::function<void(const unsigned char* data, const size_t dataSize)> m_serverDataCallback;

    float m_time;
    float m_inputDelay;
    float m_serverDelay;
    float m_packetLoss;
};

#endif /* FakeNet_h */
