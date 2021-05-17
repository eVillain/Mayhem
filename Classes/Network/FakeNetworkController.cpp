#include "FakeNetworkController.h"
#include "Message.h"
#include "ReadStream.h"
#include "WriteStream.h"
#include "FakeNet.h"
#include "DrudgeNet.h"
#include "NetworkMessageFactory.h"

const int FakeNetworkController::BUFFER_SIZE = 16 * 1024;

FakeNetworkController::FakeNetworkController(std::shared_ptr<FakeNet> fakeNet)
: m_fakeNet(fakeNet)
, m_clientCallback(nullptr)
, m_serverCallback(nullptr)
, m_readBuffer(nullptr)
, m_writeBuffer(nullptr)
, m_readStream(nullptr)
, m_writeStream(nullptr)
, m_messageFactory(std::make_shared<NetworkMessageFactory>())
{
    printf("FakeNetworkController:: constructor: %p\n", this);
}

FakeNetworkController::~FakeNetworkController()
{
    printf("FakeNetworkController:: destructor: %p\n", this);
}

void FakeNetworkController::initialize(const NetworkMode mode)
{
    m_mode = mode;
    
    m_readBuffer = new unsigned char[BUFFER_SIZE];
    m_writeBuffer = new unsigned char[BUFFER_SIZE];
    
    m_readStream = std::make_shared<Net::ReadStream>(m_readBuffer, BUFFER_SIZE);
    m_writeStream = std::make_shared<Net::WriteStream>(m_writeBuffer, BUFFER_SIZE);
    
    if (m_mode == NetworkMode::CLIENT)
    {
        m_fakeNet->setClientDataCallback(std::bind(&FakeNetworkController::onClientDataReceived,
                                                   this,
                                                   std::placeholders::_1,
                                                   std::placeholders::_2, std::placeholders::_3));
    }
    else
    {
        if (m_onNodeConnectedCallback)
        {
            m_onNodeConnectedCallback(0);
        }
        m_fakeNet->setServerDataCallback(std::bind(&FakeNetworkController::onServerDataReceived,
                                                   this, std::placeholders::_1, std::placeholders::_2));
    }
}

void FakeNetworkController::terminate()
{
    delete [] m_readBuffer;
    delete [] m_writeBuffer;
    m_readBuffer = nullptr;
    m_writeBuffer = nullptr;
    m_readStream = nullptr;
    m_writeStream = nullptr;
    m_fakeNet->terminate();
}

void FakeNetworkController::update(const float deltaTime)
{
    if (m_fakeNet && m_mode != NetworkMode::CLIENT)
    {
        m_fakeNet->update(deltaTime);
    }
}

float FakeNetworkController::GetRoundTripTime(const Net::NodeID nodeID)
{
    return m_fakeNet->getInputDelay() + m_fakeNet->getServerDelay();
}

Net::MessageID FakeNetworkController::sendMessage(const Net::NodeID nodeID,
                                                  std::shared_ptr<Net::Message>& message,
                                                  bool reliable /*= false*/)
{
    if (nodeID != 0)
    {
        return 0;
    }
    uint8_t messageType = message->getType();
    m_writeStream->SerializeByte(messageType);
    message->serialize(*m_writeStream);
    if (m_mode == NetworkMode::CLIENT)
    {
        m_fakeNet->takeServerData(m_writeBuffer, m_writeStream->GetDataBytes());
    }
    else
    {
        m_fakeNet->takeClientData(nodeID, m_writeBuffer, m_writeStream->GetDataBytes());
    }
    memset(m_writeBuffer, 0, m_writeStream->GetDataBytes());
    m_writeStream->Clear();
    
    return 0;
}

void FakeNetworkController::onClientDataReceived(const uint8_t playerID, const unsigned char *data, const size_t dataSize)
{
    memcpy(m_readBuffer, data, dataSize);
    
    std::shared_ptr<Net::Message> message = m_messageFactory->create(*m_readStream.get());
    if (!message)
    {
        printf("FakeNetworkController::onClientDataReceived failed to create message from player %i\n", playerID);
    }
    
    if (m_messageReceivedCallback)
    {
        m_messageReceivedCallback(message, playerID);
    }
    
    auto responder = m_dispatchMap.find(message->getType());
    if (responder != m_dispatchMap.end())
    {
        responder->second(message, playerID);
    }
    else
    {
        CCLOG("server received packetType %i (%s) from node %i with no handler\n",
              message->getType(), NetworkMessageFactory::getNameForType(message->getType()).c_str(), playerID);
    }
    
    memset(m_readBuffer, 0, dataSize);
    m_readStream->Clear();
}

void FakeNetworkController::onServerDataReceived(const unsigned char *data, const size_t dataSize)
{
    memcpy(m_readBuffer, data, dataSize);
    
    std::shared_ptr<Net::Message> message = m_messageFactory->create(*m_readStream.get());
    if (!message)
    {
        printf("FakeNetworkController::onServerDataReceived failed to create message from server\n");
    }
    
    if (m_messageReceivedCallback)
    {
        m_messageReceivedCallback(message, 0);
    }
    
    auto responder = m_dispatchMap.find(message->getType());
    if (responder != m_dispatchMap.end())
    {
        responder->second(message, 0);
    }
    else
    {
        CCLOG("client received packetType %i (%s) from node %i with no handler\n",
              message->getType(), NetworkMessageFactory::getNameForType(message->getType()).c_str(), 0);
    }
    
    memset(m_readBuffer, 0, dataSize);
    m_readStream->Clear();
}
