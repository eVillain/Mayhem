#ifndef ClientModel_h
#define ClientModel_h

#include "NetworkMessages.h"
#include <stdint.h>
#include <vector>
#include <map>

enum ClientMode {
    CLIENT_MODE_LOCAL = 0,
    CLIENT_MODE_NETWORK
};

class ClientModel
{
public:
    ClientModel();
    ~ClientModel();
    
    void reset();

    void setMode(const ClientMode mode) { m_mode = mode; }
    ClientMode getMode() const { return m_mode; }
    
    void setLocalPlayerID(const uint8_t playerID) { m_localPlayerID = playerID; }
    uint8_t getLocalPlayerID() const { return m_localPlayerID; }

    void setLocalPlayerAlive(const bool alive) { m_localPlayerAlive = alive; }
    bool getLocalPlayerAlive() const { return m_localPlayerAlive; }

    void setTicksToBuffer(const uint32_t count) { m_ticksToBuffer = count; }
    uint32_t getTicksToBuffer() const { return m_ticksToBuffer; }

    void setPredictMovement(const bool predict) { m_predictMovement = predict; }
    bool getPredictMovement() const { return m_predictMovement; }

    void setPredictBullets(const bool predict) { m_predictBullets = predict; }
    bool getPredictBullets() const { return m_predictBullets; }

    void setPredictAnimation(const bool predict) { m_predictAnimation = predict; }
    bool getPredictAnimation() const { return m_predictAnimation; }
    
    void setGameStarted(const bool started) { m_gameStarted = started; }
    bool getGameStarted() const { return m_gameStarted; }

    void setRecordSnapshots(bool recordSnapshots) { m_recordSnapshots = recordSnapshots; }
    bool getRecordSnapshots() const { return m_recordSnapshots; }

    void setLastPlayerActionTime(const float time) { m_lastPlayerActionTime = time; }
    float getLastPlayerActionTime() const { return m_lastPlayerActionTime; }
    
    std::vector<std::shared_ptr<ClientInputMessage>>& getInputData() { return m_inputData; }
    
    void setPlayerName(uint8_t playerID, const std::string name) { m_playerNames[playerID] = name; }
    const std::map<uint8_t, std::string>& getPlayerNames() const { return m_playerNames; }

private:
    ClientMode m_mode;
    uint8_t m_localPlayerID;
    bool m_localPlayerAlive;
    uint32_t m_ticksToBuffer;
    bool m_predictMovement;
    bool m_predictBullets;
    bool m_predictAnimation;
    bool m_gameStarted;
    bool m_recordSnapshots;
    float m_lastPlayerActionTime;
    std::vector<std::shared_ptr<ClientInputMessage>> m_inputData;
    std::map<uint8_t, std::string> m_playerNames;
};

#endif /* ClientModel_h */
