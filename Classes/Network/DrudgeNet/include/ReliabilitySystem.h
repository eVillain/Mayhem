#ifndef NET_RELIABILITY_H
#define NET_RELIABILITY_H

// reliability system to support reliable connection
//  + manages sent, received, pending ack and acked packet queues
//  + separated out from reliable connection because it is quite complex and i want to unit test it!
#include "PacketQueue.h"
#include <vector>
#include <functional>

namespace Net
{
    class ReliabilitySystem
    {
    public:
        
        ReliabilitySystem(PacketSequenceID max_sequence = 0xFFFFFFFF);
        
        void Reset();
        
        void PacketSent(int32_t size);
        
        void PacketReceived(PacketSequenceID sequence, int32_t size);
        
        uint32_t GenerateAckBits();
        
        void ProcessAck(uint32_t ack, uint32_t ack_bits);
        
        void Update(float deltaTime);
        
        void Validate();
        
        // utility functions
                
        static int32_t BitIndexForSequence(uint32_t sequence, uint32_t ack, uint32_t max_sequence);
        
        static uint32_t GenerateAckBits( uint32_t ack, const PacketQueue & received_queue, uint32_t max_sequence );
        
        static void ProcessAck(uint32_t ack, uint32_t ack_bits,
                               PacketQueue & pending_ack_queue, PacketQueue & acked_queue,
                               std::vector<uint32_t> & acks, uint32_t & acked_packets,
                               float & rtt, uint32_t max_sequence );
        
        // data accessors
        
        inline uint32_t GetLocalSequence() const { return local_sequence; };
        
        inline uint32_t GetRemoteSequence() const { return remote_sequence; };
        
        inline uint32_t GetMaxSequence() const { return max_sequence; };
        
        inline const std::vector<uint32_t>& GetAcks() const { return acks; };
        
        inline uint32_t GetSentPackets() const { return sent_packets; };
        
        inline uint32_t GetReceivedPackets() const { return recv_packets; };
        
        inline uint32_t GetLostPackets() const { return lost_packets; };
        
        inline uint32_t GetAckedPackets() const { return acked_packets; };
        
        inline float GetSentBandwidth() const { return sent_bandwidth; };
        
        inline float GetAckedBandwidth() const { return acked_bandwidth; }
        
        inline float GetRoundTripTime() const { return rtt; };
        
        inline int32_t GetSentTotal() const { return sent_bytes_total; };
        
        inline int32_t GetReceivedTotal() const { return recv_bytes_total; };
        
        inline int32_t GetHeaderSize() const { return 12; };
        
    protected:
        
        void AdvanceQueueTime( float deltaTime );
        
        void UpdateQueues();
        
        void UpdateStats();
        
    private:
        
        uint32_t max_sequence;			// maximum sequence value before wrap around (used to test sequence wrap at low # values)
        uint32_t local_sequence;		// local sequence number for most recently sent packet
        uint32_t remote_sequence;		// remote sequence number for most recently received packet
        
        uint32_t sent_packets;			// total number of packets sent
        uint32_t recv_packets;			// total number of packets received
        uint32_t lost_packets;			// total number of packets lost
        uint32_t acked_packets;			// total number of packets acked
        
        float sent_bandwidth;		    // approximate sent bandwidth over the last second
        float acked_bandwidth;		    // approximate acked bandwidth over the last second
        float rtt;					    // estimated round trip time
        float rtt_maximum;			    // maximum expected round trip time (hard coded to one second for the moment)
        int32_t sent_bytes_total;       // total bandwidth sent
        int32_t recv_bytes_total;       // total bandwidth received
        
        std::vector<uint32_t> acks;	    // acked packets from last set of packet receives. cleared each update!
        
        PacketQueue sentQueue;          // sent packets used to calculate sent bandwidth (kept until rtt_maximum)
        PacketQueue pendingAckQueue;    // sent packets which have not been acked yet (kept until rtt_maximum * 2 )
        PacketQueue receivedQueue;      // received packets for determining acks to send (kept up to most recent recv sequence - 32)
        PacketQueue ackedQueue;         // acked packets (kept until rtt_maximum * 2)
    };
}

#endif
