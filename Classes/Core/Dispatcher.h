#ifndef Dispatcher_h
#define Dispatcher_h

#include "Event.h"
#include <functional>
#include <map>
#include <vector>

class Dispatcher
{
public:
    static Dispatcher& globalDispatcher() {
        return g_dispatcher;
    }
    
    using ListenerType = std::function<void(const Event&)>;
    
    void addListener(const Event::DescriptorType& descriptor, ListenerType listener);
    void removeListeners(const Event::DescriptorType& descriptor);

    void dispatch(const Event& event) const;
    
private:
    static Dispatcher g_dispatcher;

    std::map<Event::DescriptorType, std::vector<ListenerType>> _listeners;
};
#endif /* Dispatcher_h */
