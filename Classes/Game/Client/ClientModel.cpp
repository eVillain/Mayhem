#include "ClientModel.h"

#include "SharedConstants.h"

ClientModel::ClientModel()
: m_mode(ClientMode::CLIENT_MODE_LOCAL)
, m_localPlayerID(0)
, m_localPlayerAlive(false)
, m_interpolationLatency(DEFAULT_INTERPOLATION_LATENCY)
, m_predictMovement(true)
, m_predictBullets(true)
, m_predictAnimation(true)
, m_gameStarted(false)
, m_recordSnapshots(false)
, m_lastPlayerActionTime(-1.f)
{
    printf("ClientModel:: constructor: %p\n", this);
}

ClientModel::~ClientModel()
{
    printf("ClientModel:: destructor: %p\n", this);
}

void ClientModel::reset()
{
    m_mode = ClientMode::CLIENT_MODE_LOCAL;
    m_localPlayerID = 0;
    m_localPlayerAlive = false;
    m_interpolationLatency = DEFAULT_INTERPOLATION_LATENCY;
    m_predictMovement = true;
    m_predictBullets = true;
    m_predictAnimation = true;
    m_gameStarted = false;
    m_recordSnapshots = false;
    m_lastPlayerActionTime = -1.f;
    
    m_inputData.clear();
    m_playerNames.clear();
}
