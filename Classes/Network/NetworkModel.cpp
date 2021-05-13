#include "NetworkModel.h"

NetworkModel::NetworkModel()
: m_hostName("Mayhem Royale Server")
, m_protocol(666)
, m_listenPort(20001)
, m_hostPort(20002)
, m_beaconPort(20003)
, m_beaconListenerPort(20004)
, m_listenerTimeout(10.f)
{
}
