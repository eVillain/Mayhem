#ifndef NET_ADDRESS_H
#define NET_ADDRESS_H

#include "DataTypes.h"
#include <string>

#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
#include <arpa/inet.h>
#include <netdb.h>
#elif PLATFORM == PLATFORM_WINDOWS
#include <WS2tcpip.h>
#endif

namespace Net
{
    class Address
    {
    public:
        static bool validateIpAddress(const std::string& ipAddress)
        {
            struct sockaddr_in sa;
            int result = inet_pton(AF_INET, ipAddress.c_str(), &(sa.sin_addr));
            return result != 0;
        }
        
        Address(const std::string& hostNameWithPort)
        {
            const size_t colon = hostNameWithPort.find(":") + 1;
            const std::string port = hostNameWithPort.substr(colon, hostNameWithPort.length() - colon);
            const std::string host = hostNameWithPort.substr(0, colon - 1);
            initWithHost(host, port);
        }

        Address(const std::string& hostName,
                const std::string& port)
        {
            initWithHost(hostName, port);
        }
        
        Address()
        : m_address(0)
        , m_port(0)
        {}
        
        Address(const uint8_t a,
                const uint8_t b,
                const uint8_t c,
                const uint8_t d,
                Port port) :
        m_address((a << 24) | (b << 16) | (c << 8) | d),
        m_port(port)
        {}
        
        Address(const uint32_t address, const uint16_t port)
        {
            m_address = address;
            m_port = port;
        }
        
        uint32_t GetAddress() const { return m_address; }
        uint8_t GetA() const { return (uint8_t)(m_address >> 24); }
        uint8_t GetB() const { return (uint8_t)(m_address >> 16); }
        uint8_t GetC() const { return (uint8_t)(m_address >> 8); }
        uint8_t GetD() const { return (uint8_t)(m_address); }
        
        Port GetPort() const { return m_port; }
        
        std::string GetString() const
        {
            return (std::to_string(GetA()) + "." +
                    std::to_string(GetB()) + "." +
                    std::to_string(GetC()) + "." +
                    std::to_string(GetD()) + ":" +
                    std::to_string(GetPort()));
        }
        
        bool operator == (const Address& other) const
        {
            return m_address == other.m_address && m_port == other.m_port;
        }
        
        bool operator != (const Address& other) const
        {
            return !(*this == other);
        }
        
        // note: required so we can use an Address as a key in std::map
        bool operator < (const Address& other) const
        {
            if (m_address < other.m_address)
            {
                return true;
            }
            if (m_address > other.m_address)
            {
                return false;
            }
            else
            {
                return (m_port < other.m_port);
            }
        }
    private:
        uint32_t m_address;
        Port m_port;
        
        void initWithHost(const std::string& host, const std::string& port)
        {
            struct addrinfo hints = {0}, *addrs;
            hints.ai_family = AF_UNSPEC;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;

            const int status = getaddrinfo(host.c_str(), port.c_str(), &hints, &addrs);
            if (status != 0)
            {
                std::string errorString = gai_strerror(status);
                printf("Address::initWithHost error %s", errorString.c_str());
            }
            
            char _address[INET6_ADDRSTRLEN];
            if (addrs->ai_family != AF_INET)
            {
                printf("Address::initWithHost error wrong family for %s", host.c_str());
                return;
            }
            if (inet_ntop(AF_INET, &((struct sockaddr_in*)addrs->ai_addr)->sin_addr, _address, sizeof(_address)) == nullptr)
             {
               printf("Address::initWithHost error getting address for %s", host.c_str());
               return;
             }
            
            const std::string ipAddress = _address;
            initWithIPAddress(ipAddress, port);
        }
        
        void initWithIPAddress(const std::string& address, const std::string& port)
        {
            int addressValues[4] = {0,0,0,0};
            size_t dotPos = 0;
            for (int i = 0; i < 4; i++)
            {
                size_t nextDot = address.find(".", dotPos);
                if (nextDot == 0)
                {
                    printf("Address::initWithIPAddress error address cant start with a dot %s", address.c_str());
                    return;
                }
                
                std::string value = address.substr(dotPos, (nextDot - dotPos));
                addressValues[i] = std::stoi(value);
                
                dotPos = nextDot + 1;
            }
            
            m_address = (addressValues[0] << 24) | (addressValues[1] << 16) | (addressValues[2] << 8) | addressValues[3];
            m_port = std::stoi(port);
        }
    };
}

#endif
