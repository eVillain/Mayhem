#ifndef InputCache_h
#define InputCache_h

#include "Network/NetworkMessages.h"
#include <map>
#include <queue>

struct ServerInput {
    float gameTime;
    std::shared_ptr<ClientInputMessage> clientInput;
};

class InputCache
{
public:
    InputCache();
    ~InputCache();

    void onInputDataReceived(uint8_t playerID,
                             const std::shared_ptr<ClientInputMessage>& data,
                             const float gameTime);
    void clear();
    
    std::map<uint8_t, ServerInput> popCombinedInputs();

    std::map<uint8_t, std::queue<ServerInput>>& getInputData() { return m_inputs; }
    const uint32_t getLastReceivedSequence(const uint8_t playerID) const;
    const uint32_t getLastReceivedSnapshot(const uint8_t playerID) const;
    const uint32_t getLastAppliedSequence(const uint8_t playerID) const;

    bool hasReceivedSequence(const uint8_t playerID) const { return m_lastReceivedSequences.find(playerID) != m_lastReceivedSequences.end(); }
    bool hasReceivedSnapshot(const uint8_t playerID) const { return m_lastReceivedSnapshots.find(playerID) != m_lastReceivedSnapshots.end(); }
private:
    // PlayerID, Data
    std::map<uint8_t, std::queue<ServerInput>> m_inputs;
    std::map<uint8_t, uint32_t> m_lastReceivedSequences;
    std::map<uint8_t, uint32_t> m_lastReceivedSnapshots;
    std::map<uint8_t, uint32_t> m_lastAppliedSequences;
};

#endif /* InputCache_h */
