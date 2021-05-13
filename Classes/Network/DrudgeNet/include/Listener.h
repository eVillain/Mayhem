#ifndef NET_LISTENER_H
#define NET_LISTENER_H

#include "Socket.h"
#include "Network/DrudgeNet/include/DataTypes.h"
#include <assert.h>
#include <vector>

namespace Net
{
    // Listener entry
    //  + an entry in the list of Servers on the LAN
    
    struct ListenerEntry {
        char name[64+1];
        Address address;
        float timeoutAccumulator;
    };
    
    // Listener
    //  + listens for broadcast packets sent over the LAN
    //  + use a listener to get a list of all the Servers on the LAN
    
    class Listener
    {
    public:
        
        Listener(ProtocolID protocolId, float timeout);
        
        ~Listener();
        
        bool Start(Port port);
        
        void Stop();
        
        void Update(double deltaTime);
        
        int GetEntryCount() const { return (int)entries.size(); };
        
        const ListenerEntry& GetEntry(int index) const {
            assert(index >= 0);
            assert(index < (int)entries.size());
            return entries[index];
        };
        
    protected:
        ListenerEntry* FindEntry(const ListenerEntry& entry);
        
    private:
        void ClearData() { entries.clear(); };
        
        std::vector<ListenerEntry> entries;
        ProtocolID protocolId;
        float timeout;
        bool running;
        Socket socket;
    };
}

#endif
