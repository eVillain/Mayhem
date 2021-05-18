#include "Socket.h"

#include <stdio.h>
#include <cassert>
#include <algorithm>

#ifdef _WIN32
#else
#include <unistd.h>
#endif

namespace Net
{
	bool Socket::s_socketsInitialized = false;

    bool Socket::InitializeSockets()
    {
        if (s_socketsInitialized)
        {
            return true;
        }
        s_socketsInitialized = true;
#if PLATFORM == PLATFORM_WINDOWS
        WSADATA WsaData;

        int err_code = WSAStartup( MAKEWORD(2,2), &WsaData ) != NO_ERROR;

        // I get error from outputdebug string using sysinternal debugview tool.
        switch(err_code)
        {
            case WSASYSNOTREADY:
                OutputDebugStringW(L"The underlying network subsystem is not ready for network communication.");
                break;
            case WSAVERNOTSUPPORTED:
                OutputDebugStringW(L"The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation.");
                break;
            case WSAEINPROGRESS:
                OutputDebugStringW(L"A blocking Windows Sockets 1.1 operation is in progress");
                break;
            case WSAEPROCLIM:
                OutputDebugStringW(L"A limit on the number of tasks supported by the Windows Sockets implementation has been reached.");
                break;
            case WSAEFAULT:
                OutputDebugStringW(L"The lpWSAData parameter is not a valid pointer.");
                break;
            default:
                break;
        }
        return err_code == 0;
#else
        return true;
#endif
    }

    void Socket::ShutdownSockets()
    {
    #if PLATFORM == PLATFORM_WINDOWS
        WSACleanup();
    #endif
    }

    Socket::Socket(int32_t options) :
    _options(options),
    _socket(0)
    {
		if (!s_socketsInitialized)
		{
			InitializeSockets();
		}
	}
    
    Socket::~Socket()
    {
        Close();
    }
    
    bool Socket::Open(Port port)
    {
        assert(!IsOpen());
        
        // Create socket
        _socket = ::socket(AF_INET,
                           SOCK_DGRAM,
                           IPPROTO_UDP);
        
        if (_socket <= 0)
        {
            printf("failed to create socket\n");
            _socket = 0;
            return false;
        }
        
        // Bind to port
        sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons((unsigned short)port);
        
        if (bind(_socket, (const sockaddr*)& address, sizeof(sockaddr_in)) < 0)
        {
            printf("failed to bind socket\n");
            Close();
            return false;
        }
        
        // Set non-blocking IO
        if (_options & NonBlocking)
        {
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
            int nonBlocking = 1;
            if (fcntl(_socket, F_SETFL, O_NONBLOCK, nonBlocking) == -1)
            {
                printf("failed to set non-blocking socket\n");
                Close();
                return false;
            }
#elif PLATFORM == PLATFORM_WINDOWS
            DWORD nonBlocking = 1;
            if (ioctlsocket(_socket, FIONBIO, &nonBlocking) != 0)
            {
                printf("failed to set non-blocking socket\n");
                Close();
                return false;
            }
#endif
        }
        
        // Set broadcast socket
        if (_options & Broadcast)
        {
            int enable = 1;
            if (setsockopt(_socket, SOL_SOCKET, SO_BROADCAST, (const char*)& enable, sizeof(enable)) < 0)
            {
                printf("failed to set socket to broadcast\n");
                Close();
                return false;
            }
        }
        
        return true;
    }
    
    void Socket::Close()
    {
        if (_socket != 0)
        {
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
            close(_socket);
#elif PLATFORM == PLATFORM_WINDOWS
            closesocket(_socket);
#endif
            _socket = 0;
        }
    }
    
    bool Socket::IsOpen() const
    {
        return _socket != 0;
    }
    
    bool Socket::Send(const Address& destination, const void* data, size_t size)
    {
        assert(data);
        assert(size > 0);
        
        if (_socket == 0)
            return false;
        
        assert(destination.GetAddressIP4() != 0);
        assert(destination.GetPort() != 0);
        
        sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl( destination.GetAddressIP4() );
        address.sin_port = htons( (unsigned short) destination.GetPort() );
        
        int sent_bytes = (int)sendto(_socket,
                                     (const char*)data,
                                     size,
                                     0,
                                     (sockaddr*)&address,
                                     sizeof(sockaddr_in));
        return sent_bytes == size;
    }
    
    int64_t Socket::Receive(Address& sender, void* data, size_t size)
    {
        assert(data);
        assert(size > 0);
        
        if (_socket == 0)
            return false;
        
#if PLATFORM == PLATFORM_WINDOWS
        typedef int socklen_t;
#endif
        
        sockaddr_in from;
        socklen_t fromLength = sizeof( from );
        
        int64_t received_bytes = recvfrom(_socket,
                                         (char*)data,
                                         size,
                                         0,
                                         (sockaddr*)&from,
                                         &fromLength);
        
        if (received_bytes <= 0)
            return 0;
        
        uint32_t address = ntohl( from.sin_addr.s_addr );
        uint16_t port = ntohs( from.sin_port );
        
        sender = Address(address, port);

        return received_bytes;
    }
}

