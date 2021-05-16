#ifndef INJECTOR_H
#define INJECTOR_H

#include <unordered_map>
#include <memory>
#include <functional>
#include <cassert>
#include <mutex>
#include <typeinfo>

using std::shared_ptr;
using std::make_shared;
using std::function;
using std::pair;
using std::unordered_map;
using std::size_t;
using std::recursive_mutex;
using std::lock_guard;

/**
 * Injector
 * Inversion-of-Control Dependency Injector
 * Uses variadic templates to provide dependencies at compile-time.
*/
class Injector
{
public:
    static Injector& globalInjector() { return g_injector; }
    
    /**
     * Does this injector have a mapping for the given type?
     *
     * @return True if a mapping exists for the given type, otherwise false.
     */
    template <typename T>
    bool hasMapping()
    {
        return hasTypeToInstanceMapping<T>() | hasTypeToFactoryMapping<T>() | hasTypeToInstanceMapping<T>();
    }
    
    /**
     * Maps a class in the injector. A unique instance of the mapped class
     * will be provided for each injection request.
     * Mapping a previously mapped class will replace the old mapping.
     *
     * @return A reference to this injector so it can be used in a fluent interface.
     */
    template <typename T, typename... Dependencies>
    Injector& map()
    {
        lock_guard<recursive_mutex> lockGuard{ _mutex };
        
        auto creator = [this]() -> T*
        {
            return new T(getInstance<Dependencies>()...);
        };
        
        _typesToFactories.insert(pair<size_t, function<void*()>>{typeid(T).hash_code(), creator});
        
        return *this;
    }
    
    /**
     * Maps a specific instance of an object in the injector. When an
     * object of this class is requested the provided instance will fulfill
     * the injection request.
     * Note: Since you have manually created the instance it will NOT
     * get any dependencies automatically injected into it.
     *
     * @return A reference to this injector so it can be used in a fluent interface.
     */
    template <typename T>
    Injector& mapInstance(shared_ptr<T> instance)
    {
        lock_guard<recursive_mutex> lockGuard{ _mutex };
        
        shared_ptr<IHolder> holder = shared_ptr<Holder<T>>{ new Holder<T>{ instance } };
        
        _typesToInstances.insert(pair<size_t, shared_ptr<IHolder>>{typeid(T).hash_code(), holder});
        
        return *this;
    }
    
    /**
     * Maps a class as a singleton in the injector. A single instance
     * will be used to fulfill every injection request for this class.
     *
     * @return A reference to this injector so it can be used in a fluent interface.
     */
    template <typename T, typename... Dependencies>
    Injector& mapSingleton()
    {
        lock_guard<recursive_mutex> lockGuard{ _mutex };
        
        auto instance = make_shared<T>(getInstance<Dependencies>()...);
        
        return mapInstance<T>(instance);
    }
    
    /**
     * Maps a class as a singleton of an interface class. When an object
     * of the interface class is requested a single instance of the
     * concrete class will fulfill the injection request.
     *
     * @return A reference to this injector so it can be used in a fluent interface.
     */
    template <typename Interface, typename ConcreteClass, typename... Dependencies>
    Injector& mapSingletonOf()
    {
        lock_guard<recursive_mutex> lockGuard{ _mutex };
        
        // Ensure we have the concrete class registered as a singleton first
        if (!hasTypeToInstanceMapping<ConcreteClass>())
        {
            mapSingleton<ConcreteClass, Dependencies...>();
        }
        
        auto instanceGetter = [this]() -> shared_ptr<IHolder>
        {
            auto instance = getInstance<ConcreteClass>();
            shared_ptr<IHolder> holder = shared_ptr<Holder<Interface>>{ new Holder<Interface>{ instance } };
            
            return holder;
        };
        
        _interfacesToInstanceGetters.insert(pair<size_t, function<shared_ptr<IHolder>()>>{typeid(Interface).hash_code(), instanceGetter});
        
        return *this;
    }
    
    /**
     * Maps a class as an interface of a previously mapped singleton. When
     * an object of the interface class is requested a single instance of the
     * concrete class will fulfill the injection request.
     *
     * @return A reference to this injector so it can be used in a fluent interface.
     */
    template <typename Interface, typename RegisteredConcreteClass>
    Injector& mapInterfaceToType()
    {
        lock_guard<recursive_mutex> lockGuard{ _mutex };
        
        // Ensure we have the concrete class registered as a singleton first
        if (!hasTypeToInstanceMapping<RegisteredConcreteClass>() &&
            !hasTypeToFactoryMapping<RegisteredConcreteClass>())
        {
            assert(false && "One of your injected dependencies isn't mapped, please check your mappings.");
        }
        
        auto instanceGetter = [this]() -> shared_ptr<IHolder>
        {
            auto instance = getInstance<RegisteredConcreteClass>();
            shared_ptr<IHolder> holder = shared_ptr<Holder<Interface>>{ new Holder<Interface>{ instance } };
            
            return holder;
        };
        
        _interfacesToInstanceGetters.insert(pair<size_t, function<shared_ptr<IHolder>()>>{typeid(Interface).hash_code(), instanceGetter});
        
        return *this;
    }
    
    /**
     * Returns an instance of the given type if a mapping exists.
     *
     * @return A mapped instance of the given type if a mapping exists, otherwise nullptr.
     */
    template <typename T>
    shared_ptr<T> getInstance()
    {
        lock_guard<recursive_mutex> lockGuard{ _mutex };
        
        // Try getting registered singleton or instance.
        if (hasTypeToInstanceMapping<T>())
        {
            // get as reference to avoid refcount increment
            auto& iholder = _typesToInstances[typeid(T).hash_code()];
            
            auto holder = dynamic_cast<Holder<T>*>(iholder.get());
            return holder->_instance;
        } // Otherwise attempt getting the creator and act as factory.
        else if (hasTypeToFactoryMapping<T>())
        {
            auto& creator = _typesToFactories[typeid(T).hash_code()];
            
            return shared_ptr<T>{(T*)creator()};
        }
        else if (hasInterfaceToInstanceMapping<T>())
        {
            auto& instanceGetter = _interfacesToInstanceGetters[typeid(T).hash_code()];
            
            auto iholder = instanceGetter();
            
            auto holder = dynamic_cast<Holder<T>*>(iholder.get());
            return holder->_instance;
        }
        
        // If you debug, in some debuggers (e.g Apple's lldb in Xcode) it will breakpoint in this assert
        // and by looking in the stack trace you'll be able to see which class you forgot to map.
        assert(false && "One of your injected dependencies isn't mapped, please check your mappings.");
        
        return nullptr;
    }
    
    /**
     * Creates an instance of the given type and resolves its
     * dependencies without mapping it.
     *
     * @return An unmapped instance of the given type.
     */
    template <typename T, typename... Dependencies>
    shared_ptr<T> instantiateUnmapped()
    {
        lock_guard<recursive_mutex> lockGuard{ _mutex };

        return std::make_shared<T>(getInstance<Dependencies>()...);
    }
    
    /**
     * Removes the mapping of the given type it one exists.
     *
     * @return True if a mapping was removed, otherwise false.
     */
    template <typename T>
    bool removeMapping()
    {
        lock_guard<recursive_mutex> lockGuard{ _mutex };

        bool removed = false;
        if (hasTypeToInstanceMapping<T>())
        {
            _typesToInstances.erase(typeid(T).hash_code());
            removed = true;
        }
        else if (hasTypeToFactoryMapping<T>())
        {
            _typesToFactories.erase(typeid(T).hash_code());
            removed = true;
        }
        else if (hasInterfaceToInstanceMapping<T>())
        {
            _interfacesToInstanceGetters.erase(typeid(T).hash_code());
            removed = true;
        }

        return removed;
    }

private:
    static Injector g_injector;

    struct IHolder
    {
        virtual ~IHolder() = default;
    };
    
    template <typename T>
    struct Holder : public IHolder
    {
        Holder(shared_ptr<T> instance) : _instance(instance)
        {}
        
        shared_ptr<T> _instance;
    };
    
    // Holds instances - keeps singletons and custom registered instances
    unordered_map<size_t, shared_ptr<IHolder>> _typesToInstances;
    // Holds creators used to instantiate a type
    unordered_map<size_t, function<void*()>> _typesToFactories;
    // Holds interface mappings used to get concrete instances
    unordered_map<size_t, function<shared_ptr<IHolder>()>> _interfacesToInstanceGetters;
    
    recursive_mutex _mutex;
    
    // Check if we have a mapped singleton or instance.
    template <typename T>
    inline bool hasTypeToInstanceMapping() {
        return _typesToInstances.find(typeid(T).hash_code()) != _typesToInstances.end();
    }
    // Check if we have a mapped factory.
    template <typename T>
    inline bool hasTypeToFactoryMapping() {
        return _typesToFactories.find(typeid(T).hash_code()) != _typesToFactories.end();
    }
    // Check if we have an interface type mapped to an instance.
    template <typename T>
    inline bool hasInterfaceToInstanceMapping() {
        return _interfacesToInstanceGetters.find(typeid(T).hash_code()) != _interfacesToInstanceGetters.end();
    }
};

#endif
