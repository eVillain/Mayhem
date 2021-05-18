#ifndef NET_SOCKET_H
#define NET_SOCKET_H

#include "SocketPlatform.h"
#include "Address.h"

namespace Net
{
    class Socket
    {
    public:
        static bool InitializeSockets();
        static void ShutdownSockets();
        
        enum SocketOptions {
            NonBlocking = 1,
            Broadcast = 2
        };
        
        Socket(const int32_t options = NonBlocking);
        
        ~Socket();
        
        bool Open(Port port);
        
        void Close();
        
        bool IsOpen() const;
        
        bool Send(const Address& destination, const void* data, size_t size);
        
        int64_t Receive(Address& sender, void* data, size_t size);
        
        static bool validateIpAddress(const std::string& ipAddress)
        {
            struct sockaddr_in sa;
            int result = inet_pton(AF_INET, ipAddress.c_str(), &(sa.sin_addr));
            return result != 0;
        }
        
    private:
        int32_t _socket;
        int32_t _options;

		static bool s_socketsInitialized;
    };
}

#endif
