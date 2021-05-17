#include "InputCache.h"

InputCache::InputCache()
{
    printf("[Server]InputCache:: constructor: %p\n", this);
}

InputCache::~InputCache()
{
    printf("[Server]InputCache:: destructor: %p\n", this);
}

void InputCache::onInputDataReceived(uint8_t playerID,
                                     const std::shared_ptr<ClientInputMessage>& data,
                                     const float gameTime)
{
    ServerInput input = { gameTime, data };
    m_inputs[playerID].push_back(input);
    m_lastReceivedSequences[playerID] = data->inputSequence;
    m_lastReceivedSnapshots[playerID] = data->lastReceivedSnapshot;
}

void InputCache::clear()
{
    m_inputs.clear();
    m_lastReceivedSequences.clear();
    m_lastReceivedSnapshots.clear();
}

std::map<uint8_t, ServerInput> InputCache::getCombinedInputs()
{
    std::map<uint8_t, ServerInput> output;
    for (const auto& input : m_inputs)
    {
        const uint8_t playerID = input.first;
        const auto& playerInputs = input.second;
        for (const auto& playerInput : playerInputs)
        {
            auto it = output.find(playerID);
            if (it == output.end())
            {
                auto inputCopy = std::make_shared<ClientInputMessage>(*playerInput.clientInput.get());
                output[playerID] = {playerInput.gameTime, inputCopy};
            }
            else
            {
                (*it).second.gameTime = playerInput.gameTime;
                (*it).second.clientInput->inputSequence = playerInput.clientInput->inputSequence;
                (*it).second.clientInput->lastReceivedSnapshot = playerInput.clientInput->lastReceivedSnapshot;
                (*it).second.clientInput->directionX = playerInput.clientInput->directionX;
                (*it).second.clientInput->directionY = playerInput.clientInput->directionY;
                (*it).second.clientInput->aimPointX = playerInput.clientInput->aimPointX;
                (*it).second.clientInput->aimPointY = playerInput.clientInput->aimPointY;

                (*it).second.clientInput->shoot |= playerInput.clientInput->shoot;
                (*it).second.clientInput->interact |= playerInput.clientInput->interact;
                (*it).second.clientInput->run |= playerInput.clientInput->run;
                (*it).second.clientInput->reload |= playerInput.clientInput->reload;
                (*it).second.clientInput->changeWeapon |= playerInput.clientInput->changeWeapon;
                (*it).second.clientInput->slot = playerInput.clientInput->slot;
                
                (*it).second.clientInput->pickUpID = playerInput.clientInput->pickUpID;
                (*it).second.clientInput->pickUpType = playerInput.clientInput->pickUpType;
                (*it).second.clientInput->pickUpAmount = playerInput.clientInput->pickUpAmount;
            }
        }
    }
    
    return output;
}

const uint32_t InputCache::getLastReceivedSequence(const uint8_t playerID) const
{
    auto it = m_lastReceivedSequences.find(playerID);
    if (it == m_lastReceivedSequences.end())
    {
        return 0;
    }
    return it->second;
    
}
const uint32_t InputCache::getLastReceivedSnapshot(const uint8_t playerID) const
{
    auto it = m_lastReceivedSnapshots.find(playerID);
    if (it == m_lastReceivedSnapshots.end())
    {
        return 0;
    }
    return it->second;
}
