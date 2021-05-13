#include "Beacon.h"
#include "Socket.h"
#include "Serialization.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

namespace Net
{
    Beacon::Beacon(const std::string& hostName,
                   const ProtocolID protocol,
                   const Port listenPort,
                   const Port hostPort)
    : socket(Socket::Broadcast | Socket::NonBlocking)
    , state(State::Stopped)
    , name(hostName)
    , protocolID(protocol)
    , listenerPort(listenPort)
    , serverPort(hostPort)
    , running(false)
    , timeAccumulator(0)
    {
        assert(name.length() < 65);
    }
    
    Beacon::~Beacon()
    {
        if (running)
        {
            Stop();
        }
    }
    
    bool Beacon::Start(Port port)
    {
        assert(!running);
        if (!socket.Open(port))
        {
            return false;
        }
        running = true;
        return true;
    }
    
    void Beacon::Stop()
    {
        assert(running);
        socket.Close();
        running = false;
    }
    
    void Beacon::Update(double delta)
    {
        assert(running);
        timeAccumulator += delta;
        if (timeAccumulator >= 1.0f)
        {
            // Broadcast beacon advertising server
            unsigned char *packet;
            packet = new unsigned char[12+1+64];
            Serialization::WriteInteger(packet, 0);
            Serialization::WriteInteger(packet + 4, protocolID);
            Serialization::WriteShort(packet + 8, serverPort);
            packet[12] = (unsigned char) name.length();
            assert(packet[12] < 63);
            memcpy(packet + 13, name.c_str(), name.length());
            
            if (!socket.Send(Address(255,255,255,255,listenerPort), packet, 12 + 1 + packet[12]))
            {
                state = State::Failed;
            }
            else
            {
                state = State::Running;
            }
            
            delete [] packet;
            timeAccumulator -= 1.0f;
        }
    }
}

