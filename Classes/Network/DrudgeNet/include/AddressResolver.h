#ifndef NET_ADDRESS_RESOLVER_H
#define NET_ADDRESS_RESOLVER_H

#include "Socket.h"
#include <string>

#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
#include <arpa/inet.h>
#include <netdb.h>
#elif PLATFORM == PLATFORM_WINDOWS
#include <WS2tcpip.h>
#endif

namespace Net
{
    class AddressResolver
    {
    public:
        static Address getAddressForHost(const std::string& host,
                                         const std::string& port)
        {
            if (!Socket::InitializeSockets())
            {
                return Address();
            }
            struct addrinfo hints = {0}, *addrs;
            hints.ai_family = AF_UNSPEC;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;

            const int status = getaddrinfo(host.c_str(), port.c_str(), &hints, &addrs);
            if (status != 0)
            {
#if PLATFORM == PLATFORM_WINDOWS
                std::wstring ws = gai_strerror(status);
                std::string errorString = std::string(ws.begin(), ws.end());
#else
                std::string errorString = gai_strerror(status);
#endif
                printf("AddressResolver::getAddressForHost error %s", errorString.c_str());
                return Address();
            }
            
            char _address[INET6_ADDRSTRLEN];
            if (addrs->ai_family != AF_INET)
            {
                printf("AddressResolver::getAddressForHost error wrong family for %s", host.c_str());
                return Address();
            }
            if (inet_ntop(AF_INET, &((struct sockaddr_in*)addrs->ai_addr)->sin_addr, _address, sizeof(_address)) == nullptr)
             {
               printf("AddressResolver::getAddressForHost error getting address for %s", host.c_str());
               return Address();
             }
            
            const std::string ipAddress = _address;
            return Address(getIP4AddressFromString(ipAddress), getPortFromString(port));
        }
        
        static uint32_t getIP4AddressFromString(const std::string& address)
        {
            int addressValues[4] = {0,0,0,0};
            size_t dotPos = 0;
            for (int i = 0; i < 4; i++)
            {
                size_t nextDot = address.find(".", dotPos);
                if (nextDot == 0)
                {
                    printf("AddressResolver::initWithIPAddress error address cant start with a dot %s", address.c_str());
                    return 0;
                }
                
                std::string value = address.substr(dotPos, (nextDot - dotPos));
                addressValues[i] = std::stoi(value);
                
                dotPos = nextDot + 1;
            }
            
            const uint32_t addressIP4 = (addressValues[0] << 24) |
                                        (addressValues[1] << 16) |
                                        (addressValues[2] << 8) |
                                        (addressValues[3]);
            return addressIP4;
        }
        
        static uint16_t getPortFromString(const std::string& port)
        {
            return std::stoi(port);
        }
    };
}

#endif
