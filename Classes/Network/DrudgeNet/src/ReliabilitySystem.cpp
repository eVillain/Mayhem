#include "ReliabilitySystem.h"

namespace Net
{
    ReliabilitySystem::ReliabilitySystem(PacketSequenceID max_sequence)
    {
        this->max_sequence = max_sequence;
        Reset();
    }
    
    void ReliabilitySystem::Reset()
    {
        local_sequence = 0;
        remote_sequence = 0;
        sentQueue.clear();
        receivedQueue.clear();
        pendingAckQueue.clear();
        ackedQueue.clear();
        sent_packets = 0;
        recv_packets = 0;
        lost_packets = 0;
        acked_packets = 0;
        sent_bandwidth = 0.0f;
        acked_bandwidth = 0.0f;
        sent_bytes_total = 0;
        recv_bytes_total = 0;
        rtt = 0.0f;
        rtt_maximum = 1.0f;
    }
    
    void ReliabilitySystem::PacketSent(int32_t size)
    {
        if (sentQueue.Exists(local_sequence))
        {
            printf("ReliabilitySystem: local sequence %d exists\n", local_sequence);
            for (PacketQueue::iterator itor = sentQueue.begin(); itor != sentQueue.end(); ++itor)
                printf(" + %d\n", itor->sequence);
        }
        assert(!sentQueue.Exists(local_sequence));
        assert(!pendingAckQueue.Exists(local_sequence));
        PacketData data;
        data.sequence = local_sequence;
        data.time = 0.0f;
        data.size = size;
        sent_bytes_total += size;
        sentQueue.push_back(data);
        pendingAckQueue.push_back(data);
        sent_packets++;
        local_sequence++;
        if (local_sequence > max_sequence)
        {
             local_sequence = 0;
        }
    }
    
    void ReliabilitySystem::PacketReceived(PacketSequenceID sequence, int32_t size)
    {
        recv_packets++;
        if (receivedQueue.Exists(sequence))
            return;
        PacketData data;
        data.sequence = sequence;
        data.time = 0.0f;
        data.size = size;
        recv_bytes_total += size;
        receivedQueue.push_back( data );
        if (IsSequenceMoreRecent(sequence, remote_sequence, max_sequence))
            remote_sequence = sequence;
    }
    
    uint32_t ReliabilitySystem::GenerateAckBits()
    {
        return GenerateAckBits(GetRemoteSequence(), receivedQueue, max_sequence);
    }
    
    void ReliabilitySystem::ProcessAck(uint32_t ack, uint32_t ack_bits)
    {
        ProcessAck(ack, ack_bits, pendingAckQueue, ackedQueue, acks, acked_packets, rtt, max_sequence);
    }
    
    void ReliabilitySystem::Update(float deltaTime)
    {
        acks.clear();
        AdvanceQueueTime(deltaTime);
        UpdateQueues();
        UpdateStats();
    }
    
    void ReliabilitySystem::Validate()
    {
        sentQueue.VerifySorted(max_sequence);
        receivedQueue.VerifySorted(max_sequence);
        pendingAckQueue.VerifySorted(max_sequence);
        ackedQueue.VerifySorted(max_sequence);
    }
    
    int32_t ReliabilitySystem::BitIndexForSequence(PacketSequenceID sequence,
                                                   uint32_t ack,
                                                   PacketSequenceID max_sequence)
    {
        assert(sequence != ack);
        assert(!IsSequenceMoreRecent(sequence, ack, max_sequence));
        if (sequence > ack)
        {
            assert(ack < 33);
            assert(max_sequence >= sequence);
            return ack + (max_sequence - sequence);
        }
        else
        {
            assert(ack >= 1);
            assert(sequence <= ack - 1);
            return ack - 1 - sequence;
        }
    }
    
    uint32_t ReliabilitySystem::GenerateAckBits(uint32_t ack,
                                                const PacketQueue & received_queue,
                                                PacketSequenceID max_sequence)
    {
        uint32_t ack_bits = 0;
        for (PacketQueue::const_iterator itor = received_queue.begin(); itor != received_queue.end(); itor++)
        {
            if (itor->sequence == ack || IsSequenceMoreRecent(itor->sequence, ack, max_sequence))
                break;
            int32_t bit_index = BitIndexForSequence(itor->sequence, ack, max_sequence);
            if (bit_index <= 31)
            {
                ack_bits |= 1 << bit_index;
            }
        }
        return ack_bits;
    }
    
    void ReliabilitySystem::ProcessAck(uint32_t ack,
                                       uint32_t ack_bits,
                                       PacketQueue & pending_ack_queue,
                                       PacketQueue & acked_queue,
                                       std::vector <uint32_t> & acks,
                                       uint32_t & acked_packets,
                                       float & rtt,
                                       PacketSequenceID max_sequence)
    {
        if (pending_ack_queue.empty())
            return;
        
        PacketQueue::iterator itor = pending_ack_queue.begin();
        while (itor != pending_ack_queue.end())
        {
            bool acked = false;
            
            if (itor->sequence == ack)
            {
                acked = true;
            }
            else if (!IsSequenceMoreRecent(itor->sequence, ack, max_sequence))
            {
                int32_t bit_index = BitIndexForSequence(itor->sequence, ack, max_sequence);
                if (bit_index <= 31)
                    acked = (ack_bits >> bit_index) & 1;
            }
            
            if (acked)
            {
                rtt += (itor->time - rtt) * 0.1f;
                acked_queue.InsertSorted(*itor, max_sequence);
                acks.push_back(itor->sequence);
                acked_packets++;
                itor = pending_ack_queue.erase(itor);
            }
            else
            {
                ++itor;
            }
        }
    }
    
    void ReliabilitySystem::AdvanceQueueTime(float deltaTime)
    {
        for (PacketQueue::iterator itor = sentQueue.begin(); itor != sentQueue.end(); itor++)
        {
            itor->time += deltaTime;
        }
        for (PacketQueue::iterator itor = receivedQueue.begin(); itor != receivedQueue.end(); itor++)
        {
            itor->time += deltaTime;
        }
        for (PacketQueue::iterator itor = pendingAckQueue.begin(); itor != pendingAckQueue.end(); itor++)
        {
            itor->time += deltaTime;
        }
        for (PacketQueue::iterator itor = ackedQueue.begin(); itor != ackedQueue.end(); itor++)
        {
            itor->time += deltaTime;
        }
    }
    
    void ReliabilitySystem::UpdateQueues()
    {
        const float epsilon = 0.001f;
        
        while (sentQueue.size() && sentQueue.front().time > rtt_maximum + epsilon)
        {
            sentQueue.pop_front();
        }
        
        if (!receivedQueue.empty())
        {
            const uint32_t latest_sequence = receivedQueue.back().sequence;
            const uint32_t minimum_sequence = latest_sequence >= 34 ? (latest_sequence - 34) : max_sequence - (34 - latest_sequence);
            while (receivedQueue.size() && !IsSequenceMoreRecent(receivedQueue.front().sequence, minimum_sequence, max_sequence))
            {
                receivedQueue.pop_front();
            }
        }
        
        while (ackedQueue.size() && ackedQueue.front().time > rtt_maximum * 2 - epsilon)
        {
            ackedQueue.pop_front();
        }
        while (pendingAckQueue.size() && pendingAckQueue.front().time > rtt_maximum + epsilon)
        {
            pendingAckQueue.pop_front();
            lost_packets++;
        }
    }
    
    void ReliabilitySystem::UpdateStats()
    {
        int32_t sent_bytes_per_second = 0;
        for (PacketQueue::iterator itor = sentQueue.begin(); itor != sentQueue.end(); ++itor)
        {
            sent_bytes_per_second += itor->size;
        }
        int32_t acked_packets_per_second = 0;
        int32_t acked_bytes_per_second = 0;
        for (PacketQueue::iterator itor = ackedQueue.begin(); itor != ackedQueue.end(); ++itor)
        {
            if (itor->time >= rtt_maximum)
            {
                acked_packets_per_second++;
                acked_bytes_per_second += itor->size;
            }
        }
        sent_bytes_per_second = (int32_t)(sent_bytes_per_second / rtt_maximum);
        acked_bytes_per_second = (int32_t)(acked_bytes_per_second / rtt_maximum);
        sent_bandwidth = sent_bytes_per_second * ( 8 / 1000.0f );
        acked_bandwidth = acked_bytes_per_second * ( 8 / 1000.0f );
    }
}
