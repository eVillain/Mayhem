#include "FakeNet.h"

#include <random>

FakeNet::FakeNet()
: m_clientDataCallback(nullptr)
, m_serverDataCallback(nullptr)
, m_time(0.f)
, m_inputDelay(0.1f)
, m_serverDelay(0.1f)
, m_packetLoss(0.f)
{
    printf("FakeNet:: constructor: %p\n", this);
}

FakeNet::~FakeNet()
{
    printf("FakeNet:: destructor: %p\n", this);
}

void FakeNet::terminate()
{
    std::queue<ClientData> emptyClientData;
    std::swap(m_clientData, emptyClientData);
    std::queue<ServerData> emptyServerData;
    std::swap(m_serverData, emptyServerData);
    m_clientDataCallback = nullptr;
    m_serverDataCallback = nullptr;
    m_time = 0.f;
    m_inputDelay = 0.1f;
    m_serverDelay = 0.1f;
    m_packetLoss = 0.f;
}

void FakeNet::update(const float deltaTime)
{
    m_time += deltaTime;

    if (m_clientDataCallback)
    {
        const float delayedInputTime = m_time - m_inputDelay;
        while (!m_clientData.empty() &&
               m_clientData.front().time <= delayedInputTime)
        {
            if (m_packetLoss <= 0.f ||
                cocos2d::random(0.f, 1.f) > m_packetLoss * 0.01f)
            {
                const auto& clientData = m_clientData.front();
                m_clientDataCallback(clientData.playerID,
                                     clientData.data,
                                     clientData.dataSize);
            }
            delete [] m_clientData.front().data;
            m_clientData.pop();
        }
    }
    if (m_serverDataCallback)
    {
        const float delayedServerTime = m_time - m_serverDelay;
        while (!m_serverData.empty() &&
               m_serverData.front().time <= delayedServerTime)
        {
            if (m_packetLoss <= 0.f ||
                cocos2d::random(0.f, 1.f) > m_packetLoss * 0.01f)
            {
                m_serverDataCallback(m_serverData.front().data,
                                     m_serverData.front().dataSize);
            }
            delete [] m_serverData.front().data;
            m_serverData.pop();
        }
    }
}

void FakeNet::takeClientData(const uint8_t playerID, const unsigned char* data, const size_t dataSize)
{
    unsigned char* localData = new unsigned char[dataSize];
    memcpy(localData, data, dataSize);
    
    ClientData d = {m_time, playerID, localData, dataSize};
    m_clientData.push(d);
}

void FakeNet::takeServerData(const unsigned char* data, const size_t dataSize)
{
    unsigned char* localData = new unsigned char[dataSize];
    memcpy(localData, data, dataSize);
    
    ServerData d = {m_time, localData, dataSize};
    m_serverData.push(d);
}

void FakeNet::setClientDataCallback(std::function<void(const uint8_t playerID, const unsigned char* data, const size_t dataSize)> cb)
{
    m_clientDataCallback = cb;
}

void FakeNet::setServerDataCallback(std::function<void(const unsigned char* data, const size_t dataSize)> cb)
{
    m_serverDataCallback = cb;
}
