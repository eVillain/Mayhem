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
        Address()
        : m_addressIP4(0)
        , m_port(0)
        {}
        
        Address(const uint8_t a,
                const uint8_t b,
                const uint8_t c,
                const uint8_t d,
                Port port)
        : m_addressIP4((a << 24) | (b << 16) | (c << 8) | d)
        , m_port(port)
        {}
        
        Address(const uint32_t address, const uint16_t port)
        {
            m_addressIP4 = address;
            m_port = port;
        }
        
        void setAddressIP4(const uint32_t address) { m_addressIP4 = address; }
        
        uint32_t GetAddressIP4() const { return m_addressIP4; }
        uint8_t GetA() const { return (uint8_t)(m_addressIP4 >> 24); }
        uint8_t GetB() const { return (uint8_t)(m_addressIP4 >> 16); }
        uint8_t GetC() const { return (uint8_t)(m_addressIP4 >> 8); }
        uint8_t GetD() const { return (uint8_t)(m_addressIP4); }
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
            return m_addressIP4 == other.m_addressIP4 && m_port == other.m_port;
        }
        
        bool operator != (const Address& other) const
        {
            return !(*this == other);
        }
        
        // note: required so we can use an Address as a key in std::map
        bool operator < (const Address& other) const
        {
            if (m_addressIP4 < other.m_addressIP4)
            {
                return true;
            }
            if (m_addressIP4 > other.m_addressIP4)
            {
                return false;
            }
            else
            {
                return (m_port < other.m_port);
            }
        }
    private:
        uint32_t m_addressIP4;
        Port m_port;
    };
}

#endif
