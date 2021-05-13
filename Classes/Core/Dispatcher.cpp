#include "Dispatcher.h"

Dispatcher Dispatcher::g_dispatcher;

void Dispatcher::addListener(const Event::DescriptorType& descriptor, ListenerType listener)
{
    _listeners[descriptor].push_back(listener);
}

void Dispatcher::removeListeners(const Event::DescriptorType& descriptor)
{
    auto it = _listeners.find(descriptor);
    if (it == _listeners.end() )
    {
        return;
    }
    
    (*it).second.clear();
}


void Dispatcher::dispatch(const Event& event) const
{
    auto type = event.type();
    
    if( _listeners.find(type) == _listeners.end() )
    {
        return;
    }
    
    auto&& listeners = _listeners.at(type);
    
    for(auto&& listener : listeners)
    {
        listener(event);
    }
}
