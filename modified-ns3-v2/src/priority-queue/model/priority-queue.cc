/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */


#include "ns3/log.h"
#include "ns3/enum.h"
#include "ns3/uinteger.h"
#include "priority-queue.h"

#include "ns3/boolean.h"
#include "ns3/pointer.h"
#include "ns3/simulator.h"

#include "ns3/ppp-header.h"
#include "ns3/udp-header.h"
#include "ns3/tcp-header.h"
#include "ns3/ipv4-header.h"

NS_LOG_COMPONENT_DEFINE ("PriorityQueue");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (PriorityQueue);

TypeId 
PriorityQueue::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::PriorityQueue")
    .SetParent<Queue> ()
    .AddConstructor<PriorityQueue> ()
    .AddAttribute ("Mode",
                   "Whether to use bytes (see MaxBytes) or packets (see MaxPackets) as the maximum queue size metric.",
                   EnumValue (QUEUE_MODE_PACKETS),
                   MakeEnumAccessor (&PriorityQueue::SetMode),
                   MakeEnumChecker (QUEUE_MODE_BYTES, "QUEUE_MODE_BYTES",
                                    QUEUE_MODE_PACKETS, "QUEUE_MODE_PACKETS"))
    .AddAttribute ("HighPriorityMaxPackets",
                   "The maximum number of packets accepted by the high priority queue.",
                   UintegerValue (100),
                   MakeUintegerAccessor (&PriorityQueue::m_highMaxPackets),
                   MakeUintegerChecker<uint32_t> ())

    .AddAttribute ("LowPriorityMaxPackets",
                   "The maximum number of packets accepted by the low priority queue.",
                   UintegerValue (100),
                   MakeUintegerAccessor (&PriorityQueue::m_lowMaxPackets),
                   MakeUintegerChecker<uint32_t> ())

    .AddAttribute ("HighPriorityMaxBytes",
                   "The maximum number of bytes accepted by the high priority queue.",
                   UintegerValue (100 * 65535),
                   MakeUintegerAccessor (&PriorityQueue::m_highMaxBytes),
                   MakeUintegerChecker<uint32_t> ())

    .AddAttribute ("LowPriorityMaxBytes",
                   "The maximum number of bytes accepted by the low priority queue.",
                   UintegerValue (100 * 65535),
                   MakeUintegerAccessor (&PriorityQueue::m_lowMaxBytes),
                   MakeUintegerChecker<uint32_t> ())

    .AddAttribute ("HighPriorityPort",
                   "The destination port number for high priority traffic.",
                   UintegerValue (3000),
                   MakeUintegerAccessor (&PriorityQueue::m_priorityPort),
                   MakeUintegerChecker<uint32_t> ())
 ;

  return tid;
}

PriorityQueue::PriorityQueue () :
  Queue (),
  m_highPackets (),
  m_bytesInHighQueue (0),
  m_lowPackets (),
  m_bytesInLowQueue (0)
{
  NS_LOG_FUNCTION_NOARGS ();
}

PriorityQueue::~PriorityQueue ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
PriorityQueue::SetMode (PriorityQueue::QueueMode mode)
{
  NS_LOG_FUNCTION (mode);
  m_mode = mode;
}

PriorityQueue::QueueMode
PriorityQueue::GetMode (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_mode;
}

uint16_t
PriorityQueue::Classify (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this << p);
  PppHeader ppp;
  p->RemoveHeader (ppp);
  Ipv4Header ip;
  p->RemoveHeader (ip);

  uint16_t priority;

  uint32_t protocol = ip.GetProtocol();
  if (protocol == 17)
    {
      UdpHeader udp;
      p->PeekHeader (udp);

      if (udp.GetDestinationPort() == m_priorityPort)
        {
          NS_LOG_INFO ("\tclassifier: high priority udp");
          //NS_LOG_UNCOND ("\tclassifier: high priority udp");
          priority = 1;
        }
      else
        {
          NS_LOG_INFO ("\tclassifier: low priority udp");
          priority = 0;
        }
    }

  else if (protocol == 6)
    {
      TcpHeader tcp;
      p->PeekHeader (tcp);
      if (tcp.GetDestinationPort() == m_priorityPort)
        {
          NS_LOG_INFO ("\tclassifier: high priority tcp");
          //NS_LOG_UNCOND ("\tclassifier: high priority tcp");
          priority = 1;
        }
      else
        {
          NS_LOG_INFO ("\tclassifier: low priority tcp");
          priority = 0;
        }
    }
  else
    {
      NS_LOG_INFO ("\tclassifier: unrecognized transport protocol");
      priority = 0;
    }

  p->AddHeader (ip);
  p->AddHeader (ppp);

  return priority;
}

bool
PriorityQueue::DoEnqueue (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this << p);

  uint16_t priority = Classify (p);

  // High Priority
  if (priority == 1)
    {
      if (m_mode == QUEUE_MODE_PACKETS && (m_highPackets.size () >= m_highMaxPackets))
        {
          NS_LOG_LOGIC ("Queue full (at max packets) -- droppping pkt");
          Drop (p);
          return false;
        }
      
      if (m_mode == QUEUE_MODE_BYTES && (m_bytesInHighQueue + p->GetSize () >= m_highMaxBytes))
        {
          NS_LOG_LOGIC ("Queue full (packet would exceed max bytes) -- droppping pkt");
          Drop (p);
          return false;
        }
      
      m_bytesInHighQueue += p->GetSize ();
      m_highPackets.push (p);
      
      NS_LOG_LOGIC ("Number packets " << m_highPackets.size ());
      NS_LOG_LOGIC ("Number bytes " << m_bytesInHighQueue);
      
      return true; 
    }

  // Low Priority
  else if (priority == 0)
    {
      if (m_mode == QUEUE_MODE_PACKETS && (m_lowPackets.size () >= m_lowMaxPackets))
        {
          NS_LOG_LOGIC ("Queue full (at max packets) -- droppping pkt");
          Drop (p);
          return false;
        }
      
      if (m_mode == QUEUE_MODE_BYTES && (m_bytesInLowQueue + p->GetSize () >= m_lowMaxBytes))
        {
          NS_LOG_LOGIC ("Queue full (packet would exceed max bytes) -- droppping pkt");
          Drop (p);
          return false;
        }
      
      m_bytesInLowQueue += p->GetSize ();
      m_lowPackets.push (p);
      
      NS_LOG_LOGIC ("Number packets " << m_lowPackets.size ());
      NS_LOG_LOGIC ("Number bytes " << m_bytesInLowQueue);
      
      return true;      
    }

  // This normally never happens unless Classify() has been changed
  else 
    {
      return false;
    }
}

Ptr<Packet>
PriorityQueue::DoDequeue (void)
{
  NS_LOG_FUNCTION (this);
  
  if (!m_highPackets.empty ())
    {
      Ptr<Packet> p = m_highPackets.front ();
      m_highPackets.pop ();
      m_bytesInHighQueue -= p->GetSize ();
      
      NS_LOG_LOGIC ("Popped " << p);
      
      NS_LOG_LOGIC ("Number packets " << m_highPackets.size ());
      NS_LOG_LOGIC ("Number bytes " << m_bytesInHighQueue);
      
      return p;
    }
  else
    {
      NS_LOG_LOGIC ("High priority queue empty");

      if (!m_lowPackets.empty ())
        {
          Ptr<Packet> p = m_lowPackets.front ();
          m_lowPackets.pop ();
          m_bytesInLowQueue -= p->GetSize ();
          
          NS_LOG_LOGIC ("Popped " << p);
          
          NS_LOG_LOGIC ("Number packets " << m_lowPackets.size ());
          NS_LOG_LOGIC ("Number bytes " << m_bytesInLowQueue);
          
          return p;
        }
      else
        {
          NS_LOG_LOGIC ("Low priority queue empty");
          return 0;
        } 
    } 
}
  
Ptr<const Packet>
PriorityQueue::DoPeek (void) const
{
  NS_LOG_FUNCTION (this);
  
  if (!m_highPackets.empty ())
    {
      Ptr<Packet> p = m_highPackets.front ();
      
      NS_LOG_LOGIC ("Number packets " << m_highPackets.size ());
      NS_LOG_LOGIC ("Number bytes " << m_bytesInHighQueue);
      
      return p;
    }
  else
    {
      NS_LOG_LOGIC ("High priority queue empty");

      if (!m_lowPackets.empty ())
        {
          Ptr<Packet> p = m_lowPackets.front ();
          
          NS_LOG_LOGIC ("Number packets " << m_lowPackets.size ());
          NS_LOG_LOGIC ("Number bytes " << m_bytesInLowQueue);
          
          return p;
        }
      else
        {
          NS_LOG_LOGIC ("Low priority queue empty");
          return 0;
        }
    }
}

} // namespace ns3
