#ifndef NETWORK_MODEL_H
#define NETWORK_MODEL_H

#include <string>

class NetworkModel
{
public:
    NetworkModel();

    void setHostName(const std::string& hostName) { m_hostName = hostName; }
    const std::string getHostName() const { return m_hostName; }

    unsigned int getProtocol() const { return m_protocol; }
    unsigned int getListenPort() const { return m_listenPort; }
    unsigned int getHostPort() const { return m_hostPort; }
    unsigned int getBeaconPort() const { return m_beaconPort; }
    unsigned int getBeaconListenerPort() const { return m_beaconListenerPort; }

    float getListenerTimeout() const { return m_listenerTimeout; }

private:
    std::string m_hostName;
    unsigned int m_protocol;
    unsigned int m_listenPort;
    unsigned int m_hostPort;
    unsigned int m_beaconPort;
    unsigned int m_beaconListenerPort;
    float m_listenerTimeout;
};

#endif // NETWORK_MODEL_H
