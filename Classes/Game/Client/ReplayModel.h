#ifndef ReplayModel_h
#define ReplayModel_h

#include "cocos2d.h"
#include "Network/NetworkMessages.h"
#include "Network/DrudgeNet/include/DataTypes.h"

class ReplayModel
{
public:
    static const std::string DEFAULT_REPLAY_FILE;
    static const std::string SETTING_SAVE_REPLAY;

    bool loadFile(const std::string& fileName);
    bool saveFile(const std::string& fileName, const uint32_t tickRate) const;

    void storeSnapshot(const SnapshotData& data);
    size_t getSnapshotIndexForTime(const float targetTime) const;
    size_t getSnapshotIndexForFrame(const uint32_t frame) const;

    uint32_t getTickRate() const { return m_tickRate; }

    std::vector<SnapshotData>& getSnapshots() { return m_snapshots; }

private:
    uint32_t m_tickRate;
    std::vector<SnapshotData> m_snapshots;
};

#endif /* ReplayModel_h */
