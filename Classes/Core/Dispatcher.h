#ifndef Dispatcher_h
#define Dispatcher_h

#include <functional>
#include <map>
#include <memory>
#include <vector>

class ICallbackContainer
{
public:
    virtual ~ICallbackContainer() {};
};

template<typename EventType>
class CallbackContainer : public ICallbackContainer
{
public:
    virtual ~CallbackContainer() {};
    CallbackContainer(std::function<void(const EventType&)> cb)
    : callback(cb)
    {}
    std::function<void(const EventType&)> callback;
};

class Dispatcher
{
public:
    static Dispatcher& globalDispatcher() {
        return g_dispatcher;
    }
    
    template<typename EventType>
    void addListener(std::function<void(const EventType&)> callback, void* listener)
    {
        static const std::size_t eventHash = typeid(EventType).hash_code();
        const std::size_t listenerHash = std::hash<void*>{}(listener);
        auto& eventListeners = m_listeners[eventHash];
        auto listenerContainer = std::make_shared<CallbackContainer<EventType>>(callback);
        eventListeners[listenerHash] = listenerContainer;
    }
    
    template<typename EventType>
    void removeListener(void* listener)
    {
        static const std::size_t eventHash = typeid(EventType).hash_code();
        const std::size_t listenerHash = std::hash<void*>{}(listener);
        m_listenersToRemove[eventHash].push_back(listenerHash);
    }

    template<typename EventType>
    void dispatch(EventType& event)
    {
        static const std::size_t eventHash = typeid(EventType).hash_code();
        auto& eventListeners = m_listeners[eventHash];
        for (auto& pair : eventListeners)
        {
            auto container = std::dynamic_pointer_cast<CallbackContainer<EventType>>(pair.second);
            if (!container->callback)
            {
                continue;
            }
            container->callback(event);
        }
        
        for (const auto& pair : m_listenersToRemove)
        {
            const std::size_t eventHash = pair.first;
            auto& eventListeners = m_listeners[eventHash];

            const auto& removedListeners = pair.second;
            for (const std::size_t listenerHash : removedListeners)
            {
                auto it = eventListeners.find(listenerHash);
                if (it != eventListeners.end())
                {
                    eventListeners.erase(it);
                }
            }
        }
        m_listenersToRemove.clear();
    }

private:
    static Dispatcher g_dispatcher;
    // Key is the event type hash, inner key is the listener pointer hash
    std::unordered_map<std::size_t, std::unordered_map<std::size_t, std::shared_ptr<ICallbackContainer>>> m_listeners;
    // Key is the event type hash, vector contains listener pointer hashes
    std::unordered_map<std::size_t, std::vector<std::size_t>> m_listenersToRemove;
    
};
#endif /* Dispatcher_h */
