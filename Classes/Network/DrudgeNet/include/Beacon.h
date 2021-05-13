#ifndef NET_BEACON_H
#define NET_BEACON_H

#include "Socket.h"
#include "Network/DrudgeNet/include/DataTypes.h"
#include <string>

namespace Net
{
    // Beacon
    //  + sends broadcast UDP packets to the LAN
    //  + use a beacon to advertise the existence of a Server

    class Beacon
    {
    public:
        enum State {
            Stopped,
            Running,
            Failed,
        };
        
        Beacon(const std::string& hostName,
               const ProtocolID protocol,
               const Port listenPort,
               const Port hostPort);
        
        ~Beacon();
        
        bool Start(Port port);
        
        void Stop();
        
        void Update(double delta);
        
    private:
        Socket socket;
        State state;
        const std::string name;
        const ProtocolID protocolID;
        const Port listenerPort;
        const Port serverPort;
        bool running;
        double timeAccumulator;
    };
}

#endif
