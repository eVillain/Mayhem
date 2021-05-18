#include "FakeNet.h"
#include <random>

FakeNet::FakeNet()
: m_clientDataCallback(nullptr)
, m_serverDataCallback(nullptr)
, m_time(0.f)
, m_inputDelay(0.1f)
, m_serverDelay(0.1f)
, m_packetLoss(0.f)
, m_totalClientBytes(0)
, m_totalServerBytes(0)
, m_lastFrameClientBytes(0)
, m_lastFrameServerBytes(0)
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
    m_totalClientBytes = 0;
    m_totalServerBytes = 0;
    m_lastFrameClientBytes = 0;
    m_lastFrameServerBytes = 0;
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
            m_lastFrameClientBytes = 0;
            if (m_packetLoss <= 0.f ||
                cocos2d::random(0.f, 1.f) > m_packetLoss * 0.01f)
            {
                m_clientDataCallback(m_clientData.front().playerID,
                                     m_clientData.front().data,
                                     m_clientData.front().dataSize);
                m_lastFrameClientBytes += m_clientData.front().dataSize;
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
            m_lastFrameServerBytes = 0;
            if (m_packetLoss <= 0.f ||
                cocos2d::random(0.f, 1.f) > m_packetLoss * 0.01f)
            {
                m_serverDataCallback(m_serverData.front().data,
                                     m_serverData.front().dataSize);
                m_lastFrameServerBytes += m_serverData.front().dataSize;
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
    
    m_totalClientBytes += dataSize;
}

void FakeNet::takeServerData(const unsigned char* data, const size_t dataSize)
{
    unsigned char* localData = new unsigned char[dataSize];
    memcpy(localData, data, dataSize);
    
    ServerData d = {m_time, localData, dataSize};
    m_serverData.push(d);
    
    m_totalServerBytes += dataSize;
}

void FakeNet::setClientDataCallback(std::function<void(const uint8_t playerID, const unsigned char* data, const size_t dataSize)> cb)
{
    m_clientDataCallback = cb;
}

void FakeNet::setServerDataCallback(std::function<void(const unsigned char* data, const size_t dataSize)> cb)
{
    m_serverDataCallback = cb;
}
