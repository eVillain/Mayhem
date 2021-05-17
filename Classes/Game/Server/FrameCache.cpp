#include "FrameCache.h"
#include "Entity.h"

const size_t DEFAULT_MAX_ROLLBACK_FRAMES = 20;

FrameCache::FrameCache()
: m_maxRollbackFrames(DEFAULT_MAX_ROLLBACK_FRAMES)
{
    printf("[Server]FrameCache:: constructor: %p\n", this);
}

FrameCache::~FrameCache()
{
    printf("[Server]FrameCache:: destructor: %p\n", this);
}

void FrameCache::takeFrameSnapshot(std::map<uint32_t, EntitySnapshot> frameData)
{
    if (m_frames.size() == m_maxRollbackFrames)
    {
        m_frames.erase(m_frames.begin());
    }

    m_frames.push_back(frameData);
}

std::map<uint32_t, EntitySnapshot>& FrameCache::rollBack(const size_t frames)
{
    assert(frames);
    assert(frames <= m_frames.size());
    
//    CCLOG("FrameCache::rollBack %zu frames of %zu cached", frames, m_frames.size());
    m_frames.resize(m_frames.size() - (frames - 1));
    return m_frames.back();
}

std::map<uint32_t, EntitySnapshot>& FrameCache::getFrame(const size_t frame)
{
    const size_t frameIndex = (m_frames.size() - 1) - (frame - 1);
    return m_frames[frameIndex];
}
