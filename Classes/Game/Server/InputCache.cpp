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
    m_inputs[playerID].push(input);
    m_lastReceivedSequences[playerID] = data->inputSequence;
    m_lastReceivedSnapshots[playerID] = data->lastReceivedSnapshot;
}

void InputCache::clear()
{
    m_inputs.clear();
    m_lastReceivedSequences.clear();
    m_lastReceivedSnapshots.clear();
    m_lastAppliedSequences.clear();
}

std::map<uint8_t, ServerInput> InputCache::popCombinedInputs()
{
    std::map<uint8_t, ServerInput> output;
    for (auto& input : m_inputs)
    {
        const uint8_t playerID = input.first;
        auto& playerInputs = input.second;
        if (playerInputs.empty())
        {
            printf("[Server]InputCache:: no input sequence for player %i", playerID);
            continue;
        }
        output[playerID] = playerInputs.front();
        playerInputs.pop();

        m_lastAppliedSequences[playerID] = output[playerID].clientInput->inputSequence;
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

const uint32_t InputCache::getLastAppliedSequence(const uint8_t playerID) const
{
    auto it = m_lastAppliedSequences.find(playerID);
    if (it == m_lastAppliedSequences.end())
    {
        return 0;
    }
    return it->second;
}
