#include "ReplayModel.h"
#include "SharedConstants.h"

#include "ReadStream.h"
#include "WriteStream.h"

const std::string ReplayModel::DEFAULT_REPLAY_FILE = "ReplayData.bin";
const std::string ReplayModel::SETTING_SAVE_REPLAY = "SaveReplay";

bool ReplayModel::loadFile(const std::string& fileName)
{
    m_snapshots.clear();
    
    auto fileUtil = cocos2d::FileUtils::getInstance();
    const std::string fullPath = fileUtil->getWritablePath() + fileName;
    if (!fileUtil->isFileExist(fullPath))
    {
        return false;
    }
    
    cocos2d::Data data = fileUtil->getDataFromFile(fullPath);
    if (data.getSize() < 10)
    {
        return false;
    }
    
    Net::ReadStream stream(data.getBytes(), data.getSize());
    
    uint64_t frameCount = 0;
    std::string HEADER;
    m_tickRate = 0;
    stream.SerializeString(HEADER);
    stream.SerializeBits(frameCount, 64);
    stream.SerializeInteger(m_tickRate);
    
    for (uint64_t frame = 0; frame < frameCount; frame++)
    {
        ServerSnapshotMessage snapshotMessage;
        snapshotMessage.serialize(stream);
        m_snapshots.push_back(snapshotMessage.data);
    }
    
    return true;
}

bool ReplayModel::saveFile(const std::string& fileName, const uint32_t tickRate) const
{
    auto fileUtil = cocos2d::FileUtils::getInstance();
    const std::string fullPath = fileUtil->getWritablePath() + fileName;
    if (fileUtil->isFileExist(fullPath))
    {
        return false;
    }
    
    int32_t bufferSize = 16*1024*1024; // 16MB should do it I guess :D
    void* bytes = malloc(bufferSize); // Will be freed by "data" below
    
    Net::WriteStream stream(bytes, bufferSize);
    
    uint64_t frameCount = m_snapshots.size();
    std::string HEADER = "MayhemReplay-v0.0/";
    uint32_t rate = tickRate;
    stream.SerializeString(HEADER);
    stream.SerializeBits(frameCount, 64);
    stream.SerializeInteger(rate);
    
    for (auto snapshot : m_snapshots)
    {
        if (stream.GetBitsRemaining() <= 8096)
        {
            break;
        }
        ServerSnapshotMessage snapshotMessage;
        snapshotMessage.data = snapshot;
        snapshotMessage.serialize(stream);
    }
        
    cocos2d::Data data; // Takes ownership of buffer
    data.fastSet((unsigned char*)bytes, stream.GetDataBytes());
    
    fileUtil->writeDataToFile(data, fullPath);
    
    return true;
}

void ReplayModel::storeSnapshot(const SnapshotData& data)
{
    m_snapshots.push_back(data);
}

size_t ReplayModel::getSnapshotIndexForTime(const float targetTime) const
{
    if (m_snapshots.empty())
    {
        return 0;
    }
    
    const float frameTime = 1.f / m_tickRate;

    float previousTime = m_snapshots.begin()->serverTick * frameTime;
    size_t targetIndex = 1;
    for (; targetIndex < m_snapshots.size() - 1; targetIndex++)
    {
        const float nextTime = m_snapshots.at(targetIndex).serverTick * frameTime;
        if (previousTime <= targetTime &&
            nextTime > targetTime)
        {
            return targetIndex;
            break;
        }
        previousTime = nextTime;
    }
    
    return 0;
}

size_t ReplayModel::getSnapshotIndexForFrame(const uint32_t frame) const
{
    for (size_t index = 0; index < m_snapshots.size(); index++)
    {
        if (m_snapshots.at(index).serverTick == frame)
        {
            return index;
        }
    }
    
    return 0;
}

