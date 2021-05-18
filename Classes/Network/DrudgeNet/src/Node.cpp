#include "Node.h"
#include "Serialization.h"
#include "CRC32.h"
#include <cassert>

namespace Net
{
    Node::Node(ProtocolID protocolId,
               float sendRate,
               float timeout,
               int32_t maxPacketSize)
    : m_protocolId(protocolId)
    , m_sendRate(sendRate)
    , m_timeout(timeout)
    , m_maxPacketSize(maxPacketSize)
    , m_state(Disconnected)
    , m_meshSendAccumulator(0.f)
    , m_timeoutAccumulator(0.f)
    , m_localNodeID(-1)
    , m_running(false)
    , m_onConnectedCallback(nullptr)
    , m_onDisconnectedCallback(nullptr)
    , m_onNodeConnectedCallback(nullptr)
    , m_onNodeDisconnectedCallback(nullptr)
    {
        ClearData();
    }
    
    Node::~Node()
    {
        if (m_running)
            Stop();
    }
    
    bool Node::Start(Port port)
    {
        assert(!m_running);
        printf("Node: starting node on port %d\n", port);
        if ( !m_socket.Open( port ) )
            return false;
        m_running = true;
        return true;
    }
    
    void Node::Stop()
    {
        assert(m_running);
        printf("Node: stopping node\n");
        ClearData();
        m_socket.Close();
        m_running = false;
    }
    
    void Node::Join(const Address& address)
    {
        printf("Node: joining mesh at %d.%d.%d.%d:%d\n",
               address.GetA(),
               address.GetB(),
               address.GetC(),
               address.GetD(),
               address.GetPort());
        ClearData();
        m_state = Joining;
        m_meshAddress = address;
    }
    
    void Node::Update(float deltaTime)
    {
        assert( m_running );
        ReceivePackets();
        SendPackets( deltaTime );
        CheckForTimeout( deltaTime );
    }
    
    bool Node::IsNodeConnected(NodeID nodeID)
    {
        assert(nodeID >= 0);
        assert(nodeID < (int)m_nodes.size());
        return m_nodes[nodeID].connected;
    }
    
    const Address& Node::GetNodeAddress(NodeID nodeID) const
    {
        assert(nodeID >= 0);
        assert(nodeID < (int)m_nodes.size());
        return m_nodes[nodeID].address;
    }
    
    int Node::GetMaxNodes() const
    {
        assert(m_nodes.size() <= 255);
        return (int)m_nodes.size();
    }
    
    bool Node::SendPacket(NodeID nodeID, const unsigned char data[], int32_t size)
    {
        assert( m_running );
        if ( m_nodes.size() == 0 )
            return false;	// not connected yet
        assert( nodeID >= 0 );
        assert( nodeID < (int) m_nodes.size() );
        if ( nodeID < 0 || nodeID >= (int) m_nodes.size() )
            return false;
        if ( !m_nodes[nodeID].connected )
            return false;
        assert( size <= m_maxPacketSize );
        if ( size > m_maxPacketSize )
            return false;
//        printf("Node %i: sent %i bytes to %i\n", m_localNodeID, size, nodeID);
        return m_socket.Send( m_nodes[nodeID].address, data, size );
    }
    
    int Node::ReceivePacket(NodeID & nodeID, unsigned char data[], int32_t size)
    {
        assert( m_running );
        if ( !m_receivedPackets.empty() )
        {
            BufferedPacket * packet = m_receivedPackets.top();
            assert( packet );
            if (packet->data.size() <= size )
            {
                nodeID = packet->nodeID;
                size = (int)packet->data.size();
                memcpy( data, &packet->data[0], size );
                delete packet;
                m_receivedPackets.pop();
                return size;
            }
            else
            {
                delete packet;
                m_receivedPackets.pop();
            }
        }
        return 0;
    }
     
    void Node::ReceivePackets()
    {
        while (true)
        {
            Address sender;
            unsigned char *data;
            data = new unsigned char[m_maxPacketSize];
            int32_t size = (int32_t)m_socket.Receive(sender, data, m_maxPacketSize);
            if (!size)
                break;
//            printf("Node %i: received %i bytes\n", m_localNodeID, size);
            ProcessPacket(sender, data, size);
        }
    }
    
    void Node::ProcessPacket( const Address & sender, unsigned char data[], int32_t size )
    {
        assert( sender != Address() );
        assert( size > 0 );
        assert( data );
        // is packet from the mesh?
        if ( sender == m_meshAddress )
        {
            ProcessMeshPacket(data, size);
        }
        else
        {
            ProcessNodePacket(sender, data, size);
        }
    }
    
    void Node::SendPackets( float deltaTime )
    {
        m_meshSendAccumulator += deltaTime;
        while ( m_meshSendAccumulator > m_sendRate )
        {
            if ( m_state == Joining )
            {
                unsigned char packet[5];
                packet[4] = 0; // node is joining: send "join request" packets
                writeCRCWithProtocolID(packet, 5);
                m_socket.Send(m_meshAddress, packet, sizeof(packet));
            }
            else if ( m_state == Joined )
            {
                unsigned char packet[5];
                packet[4] = 1; // node is joined: send "keep alive" packets
                writeCRCWithProtocolID(packet, 5);
                m_socket.Send(m_meshAddress, packet, sizeof(packet));
//                printf("Node %i: sending keepalive to mesh\n", m_localNodeID);
            }
            m_meshSendAccumulator -= m_sendRate;
        }
    }
    
    void Node::CheckForTimeout( float deltaTime )
    {
        if ( m_state == Joining || m_state == Joined )
        {
            m_timeoutAccumulator += deltaTime;
            if ( m_timeoutAccumulator > m_timeout )
            {
                if ( m_state == Joining )
                {
                    printf("Node: join failed\n");
                    m_state = JoinFail;
                    if (m_onDisconnectedCallback)
                    {
                        m_onDisconnectedCallback();
                    }
                }
                else
                {
                    printf("Node %i: node timed out, accumulator %f, delta %f\n", m_localNodeID, m_timeoutAccumulator, deltaTime);
                    m_state = Disconnected;
                    if (m_onDisconnectedCallback)
                    {
                        m_onDisconnectedCallback();
                    }
                }
                ClearData();
            }
        }
    }
    
    void Node::ClearData()
    {
        m_nodes.clear();
        m_addr2node.clear();
        while (!m_receivedPackets.empty())
        {
            BufferedPacket * packet = m_receivedPackets.top();
            delete packet;
            m_receivedPackets.pop();
        }
        m_meshSendAccumulator = 0.0f;
        m_timeoutAccumulator = 0.0f;
        m_localNodeID = -1;
        m_meshAddress = Address();
    }
    
    void Node::ProcessMeshPacket(unsigned char *data, int32_t size)
    {
//            printf("Node %i: received %i bytes from mesh\n", m_localNodeID, size);
        // *** packet sent from the mesh ***
        // ignore packets that dont have the correct protocol id
        // the first 4 bytes of each packet contain the CRC32 calculated with the protocol id
        // we replace those bytes with the correct protocol id in the packet data
        // and calculate the actual crc of the packet:
        // if the CRCs match the protocol ids must match at both ends
        uint32_t crcHeader = 0;
        Serialization::ReadInteger(data, crcHeader);
        Serialization::WriteInteger(data, m_protocolId);
        uint32_t crcPacket = 0;
        crcPacket = crc32c(crcPacket, data, size);
        if (crcHeader != crcPacket)
            return;
        // determine packet type
        enum PacketType { ConnectionAccepted, Update };
        PacketType packetType = PacketType(data[4]);
        if (packetType != ConnectionAccepted &&
            packetType != Update)
        {
            return;
        }
        // handle packet type
        switch (packetType)
        {
            case ConnectionAccepted:
            {
                if ( size != 7 )
                    return;
                if ( m_state == Joining )
                {
                    OnMeshConnectionAccepted(data[5], data[6]);
                }
                m_timeoutAccumulator = 0.0f;
            }
                break;
            case Update:
            {
                if (size != (int32_t)(5 + m_nodes.size() * 10 ))
                    return;
                if ( m_state == Joined )
                {
                    onMeshUpdate(data, size);
                }
                m_timeoutAccumulator = 0.0f;
//                printf("Node %i: mesh update received!\n", m_localNodeID);
            }
                break;
        }
    }
    
    void Node::OnMeshConnectionAccepted(const NodeID nodeID, const int32_t numNodes)
    {
        m_localNodeID = nodeID;
        m_nodes.resize(numNodes);
        printf("Node %i: joined mesh!\n", m_localNodeID);
        m_state = Joined;
        if (m_onConnectedCallback)
        {
            m_onConnectedCallback();
        }
    }
    
    void Node::onMeshUpdate(unsigned char *data, int32_t size)
    {
        unsigned char * ptr = &data[5];
        for (uint32_t i = 0; i < m_nodes.size(); ++i)
        {
            unsigned char a = ptr[0];
            unsigned char b = ptr[1];
            unsigned char c = ptr[2];
            unsigned char d = ptr[3];
            unsigned short port = (unsigned short)ptr[4] << 8 | (unsigned short)ptr[5];
            int nodeID;
            Serialization::ReadInteger(&ptr[6], (uint32_t&)nodeID);
            if ((nodeID == 0 && m_localNodeID != 0) &&
                (a == 127 && b == 0 && c == 0 && d == 1)) {
                // Localhost address, it's from the mesh localnode and needs to be fixed
                a = m_meshAddress.GetA();
                b = m_meshAddress.GetB();
                c = m_meshAddress.GetC();
                d = m_meshAddress.GetD();
            }
            Address address( a, b, c, d, port );
            if (address.GetAddressIP4() != 0)
            {
                // node is connected
                if (address != m_nodes[i].address)
                {
                    m_nodes[i].connected = true;
                    m_nodes[i].address = address;
                    m_nodes[i].nodeID = nodeID;
                    m_addr2node[address] = &m_nodes[i];
                    printf("Node %i: node %i @%d.%d.%d.%d:%i connected\n",
                           m_localNodeID,
                           m_nodes[i].nodeID,
                           m_nodes[i].address.GetA(),
                           m_nodes[i].address.GetB(),
                           m_nodes[i].address.GetC(),
                           m_nodes[i].address.GetD(),
                           m_nodes[i].address.GetPort());
                    if (m_onNodeConnectedCallback)
                    {
                        m_onNodeConnectedCallback(m_nodes[i].nodeID);
                    }
                }
            }
            else
            {
                // node is not connected
                if (m_nodes[i].connected)
                {
                    if (m_onNodeDisconnectedCallback)
                    {
                        m_onNodeDisconnectedCallback(m_nodes[i].nodeID);
                    }
                    printf("Node %i: node %i @%d.%d.%d.%d:%i disconnected\n",
                           m_localNodeID,
                           m_nodes[i].nodeID,
                           m_nodes[i].address.GetA(),
                           m_nodes[i].address.GetB(),
                           m_nodes[i].address.GetC(),
                           m_nodes[i].address.GetD(),
                           m_nodes[i].address.GetPort());
                    AddrToNode::iterator itor = m_addr2node.find(m_nodes[i].address);
                    assert( itor != m_addr2node.end() );
                    m_addr2node.erase( itor );
                    m_nodes[i].connected = false;
                    m_nodes[i].address = Address();
                }
            }
            ptr += 10;
        }
    }
    
    void Node::ProcessNodePacket(const Address& sender, unsigned char* data, int32_t size)
    {
        AddrToNode::iterator itor = m_addr2node.find(sender);
        if (itor != m_addr2node.end())
        {
            // *** packet sent from another node ***
            NodeState * node = itor->second;
            assert(node);
            int32_t nodeID = (int32_t)(node - &m_nodes[0]);
//                printf("Node %i: received package from node %i, size %i\n", m_localNodeID, nodeID, size);
            assert(nodeID >= 0 );
            assert(nodeID < (int32_t)m_nodes.size());
            
            BufferedPacket * packet = new BufferedPacket;
            packet->nodeID = nodeID;
            packet->data.resize(size);
            memcpy(&packet->data[0], data, size);
            m_receivedPackets.push(packet);
        }
    }
    
    void Node::writeCRCWithProtocolID(unsigned char* data, const uint32_t size)
    {
        Serialization::WriteInteger(data, m_protocolId);
        uint32_t crc = 0;
        crc = crc32c(crc, data, size);
        Serialization::WriteInteger(data, crc);
    }
}
