#ifndef NET_ADDRESS_H
#define NET_ADDRESS_H

#include "DataTypes.h"
#include <string>

namespace Net
{
    class Address
    {
    public:
        
        Address() :
        _address(0),
        _port(0)
        {}
        
        Address(uint8_t a,
                uint8_t b,
                uint8_t c,
                uint8_t d,
                Port port) :
        _address( ( a << 24 ) | ( b << 16 ) | ( c << 8 ) | d ),
        _port(port)
        {}
        
        Address( uint32_t address, uint16_t port )
        {
            _address = address;
            _port = port;
        }
        
        uint32_t GetAddress() const
        {
            return _address;
        }
        
        uint8_t GetA() const
        {
            return ( uint8_t ) ( _address >> 24 );
        }
        
        uint8_t GetB() const
        {
            return ( uint8_t ) ( _address >> 16 );
        }
        
        uint8_t GetC() const
        {
            return ( uint8_t ) ( _address >> 8 );
        }
        
        uint8_t GetD() const
        {
            return ( uint8_t ) ( _address );
        }
        
        Port GetPort() const
        {
            return _port;
        }
        
        std::string GetString() const
        {
            return (std::to_string(GetA()) + "." +
                    std::to_string(GetB()) + "." +
                    std::to_string(GetC()) + "." +
                    std::to_string(GetD()) + ":" +
                    std::to_string(GetPort()));
        }
        
        bool operator == ( const Address & other ) const
        {
            return _address == other._address && _port == other._port;
        }
        
        bool operator != ( const Address & other ) const
        {
            return ! ( *this == other );
        }
        
        // note: required so we can use an Address as a key in std::map
        bool operator < ( const Address & other ) const
        {
            if ( _address < other._address ) {
                return true;
            }
            if ( _address > other._address ) {
                return false;
            } else {
                return (_port < other._port);
            }
        }
    private:
        uint32_t _address;
        Port _port;
    };
}

#endif
