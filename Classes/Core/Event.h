#ifndef Event_h
#define Event_h

class Event
{
public:
    virtual ~Event();
    
    using DescriptorType = const char*;
    
    virtual DescriptorType type() const = 0;
};

#endif /* Event_h */
