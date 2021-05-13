#ifndef SnapshotModel_h
#define SnapshotModel_h

#include "cocos2d.h"
#include "Network/NetworkMessages.h"
#include "Network/DrudgeNet/include/DataTypes.h"

class SnapshotModel
{
public:
    SnapshotModel();
    ~SnapshotModel();

    static EntitySnapshot interpolateEntitySnapshot(const EntitySnapshot& from,
                                                    const EntitySnapshot& to,
                                                    const float alpha);
    static PlayerState interpolatePlayerState(const PlayerState& from,
                                              const PlayerState& to,
                                              const float alpha);

    void reset();

    void storeSnapshot(const SnapshotData& data);
    SnapshotData interpolateSnapshots(const SnapshotData& from,
                                      const SnapshotData& to,
                                      const float alpha);

    void setLastApplied(const uint32_t lastApplied) { m_lastAppliedSnapshot = lastApplied; }
    uint32_t getLastReceived() const { return m_lastReceivedSnapshot; }
    uint32_t getLastApplied() const { return m_lastAppliedSnapshot; }
    std::vector<SnapshotData>& getSnapshots() { return m_snapshots; }

    size_t getSnapshotIndexForFrame(const uint32_t frame) const;
    void eraseUpToIndex(const size_t index);

private:
    SnapshotData interpolateEntities();

    uint32_t m_lastReceivedSnapshot;
    uint32_t m_lastAppliedSnapshot;
    std::vector<SnapshotData> m_snapshots;
};

#endif /* SnapshotModel_h */
