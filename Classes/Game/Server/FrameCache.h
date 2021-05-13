#ifndef FrameCache_h
#define FrameCache_h

#include "cocos2d.h"
#include "Network/NetworkMessages.h"
#include <vector>
#include <map>
#include <memory>

class Entity;

class FrameCache
{
public:
    FrameCache();
    ~FrameCache();
    
    void setMaxRollbackFrames(const size_t frames) { m_maxRollbackFrames = frames; }
    const size_t getMaxRollbackFrames() const { return m_maxRollbackFrames; }

    const size_t getFrameCount() const { return m_frames.size(); }
    void takeFrameSnapshot(std::map<uint32_t, EntitySnapshot> frameData);
    
    std::map<uint32_t, EntitySnapshot>& rollBack(const size_t frames);
    std::map<uint32_t, EntitySnapshot>& getFrame(const size_t frame);

private:
    std::vector<std::map<uint32_t, EntitySnapshot>> m_frames;
    
    size_t m_maxRollbackFrames;
};

#endif /* FrameCache_h */
