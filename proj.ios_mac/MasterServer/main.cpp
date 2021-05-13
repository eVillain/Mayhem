#include "MasterServer.h"

#include "MasterServerConnection.h"

#include <iostream>
#include <chrono>
#include <unistd.h>

//#define TESTING_MASTER_SERVER

namespace
{
    volatile sig_atomic_t quit;

    void signal_handler(int sig)
    {
        signal(sig, signal_handler);
        quit = 1;
    }
}

int main(int argc, const char * argv[])
{
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
#ifdef SIGBREAK
    signal(SIGBREAK, signal_handler);
#endif
    
    
    const float SERVER_UPDATE_FREQUENCY = 1.f / 60.f;
    const uint64_t SERVER_UPDATE_MICROSECONDS = SERVER_UPDATE_FREQUENCY * 1000000;
    uint32_t timeAccumulator = 0;
    Net::MasterServer server;
    server.setInfoCallback([](const std::string& info){
        std::cout << info << "\n";
    });

#ifdef TESTING_MASTER_SERVER
    // Testing connections
    MasterServerConnection host(MasterServerConnection::Mode::MODE_GAME_HOST);
    MasterServerConnection client(MasterServerConnection::Mode::MODE_GAME_CLIENT, 30002);
    
    host.setInfoCallback([](const std::string& info){
        std::cout << "host: " << info << "\n";
    });
    client.setInfoCallback([](const std::string& info){
        std::cout << "client: " << info << "\n";
    });
    host.setConnectCallback([&host](){
        std::cout << "host connect\n";
        host.sendHostUpdate(100, 0, GameHostState::InLobby, 50000, true);
    });
    client.setConnectCallback([&client](){
        std::cout << "client connect\n";
        client.requestGameList(0, 100);
    });
    client.setHostListCallback([&client](const std::vector<std::shared_ptr<Net::Address>>& addresses, const uint32_t, const uint32_t){
        std::cout << "client host list received\n";
        if (addresses.empty())
        {
            return;
        }
        client.requestLinkToHost(*addresses.at(0).get(), 50001);
    });
    client.setClientConnectCallback([](const Net::Address & a, const bool success){
        std::cout << "client challenge " << a.GetString() << " success: " << success << "\n";
    });
    host.setHostConnectCallback([](const Net::Address & a, const bool success){
        std::cout << "host challenge " << a.GetString() << " success: " << success << "\n";
    });
    
    host.connect(Net::Address(127, 0, 0, 1, 30000));
    client.connect(Net::Address(127, 0, 0, 1, 30000));
#endif
    
    uint64_t startTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    uint64_t previousTime = startTime;
    
    server.start();
    
    while (!quit)
    {
        uint64_t timeNow = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        uint64_t timeDiff = timeNow - previousTime;
        timeAccumulator += timeDiff;
        previousTime = timeNow;
        
        while (timeAccumulator >= SERVER_UPDATE_MICROSECONDS)
        {
            timeAccumulator -= SERVER_UPDATE_MICROSECONDS;
            server.update(SERVER_UPDATE_FREQUENCY);
            
#ifdef TESTING_MASTER_SERVER
            host.update(SERVER_UPDATE_FREQUENCY);
            client.update(SERVER_UPDATE_FREQUENCY);
#endif
        }
        
        sleep(1);
    }
    
    server.stop();
    
    return 0;
}
