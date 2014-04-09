/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "priority-queue-model.h"

#include "ns3/log.h"
#include "ns3/boolean.h"
#include "ns3/pointer.h"
#include "ns3/simulator.h"

#include "ns3/ppp-header.h"
#include "ns3/udp-header.h"
#include "ns3/ipv4-header.h"

NS_LOG_COMPONENT_DEFINE ("PriorityQueueModel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (PriorityQueueModel);

TypeId 
PriorityQueueModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::PriorityQueueModel")
    .SetParent<Object> ()
    .AddConstructor<PriorityQueueModel> ()
    .AddAttribute ("IsEnabled", "Whether or not this PriorityQueueModel is enabled.",
		   BooleanValue (true),
		   MakeBooleanAccessor (&PriorityQueueModel::m_isEnabled),
		   MakeBooleanChecker ())
    .AddAttribute ("NodalProcessingDelay", "The constant time it takes to process a packet.",
		   TimeValue (Seconds (0)),
		   MakeTimeAccessor (&PriorityQueueModel::m_nodalProcessingDelay),
		   MakeTimeChecker ())
    .AddAttribute ("HighQueue",
                   "A queue for high priority packets.",
                   PointerValue (),
                   MakePointerAccessor (&PriorityQueueModel::m_highQueue),
                   MakePointerChecker<Queue> ())  
    .AddAttribute ("LowQueue",
                   "A queue for low priority packets.",
                   PointerValue (),
                   MakePointerAccessor (&PriorityQueueModel::m_lowQueue),
                   MakePointerChecker<Queue> ())
 ;
  return tid;
}

PriorityQueueModel::PriorityQueueModel ()
  :  m_isEnabled (true),
     m_isProcessing (false),
     m_processNext (true),
     m_nodalProcessingDelay (Seconds (0)),
     m_queueMode ('p'),
     /* Coordinated */
     m_enableThreshold (false),
     m_thresholdBytes (0),
     m_thresholdPackets (0),
     m_txQueue (0)
     /* Coordinated */

{
  NS_LOG_FUNCTION_NOARGS ();
}

PriorityQueueModel::~PriorityQueueModel ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
PriorityQueueModel::Enable (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_isEnabled = true;
}

void
PriorityQueueModel::Disable (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_isEnabled = false;
}

bool
PriorityQueueModel::IsEnabled (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_isEnabled;
}

void
PriorityQueueModel::SetNodalProcessingTime (Time t)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_nodalProcessingDelay = t;
}

Time
PriorityQueueModel::GetNodalProcessingTime (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_nodalProcessingDelay;
}

void
PriorityQueueModel::SetNetDevice (Ptr<PointToPointNetDevice> netDevice)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_netDevice = netDevice;
}

void
PriorityQueueModel::SetLowQueue (Ptr<Queue> queue)
{
  NS_LOG_FUNCTION (this << queue);
  m_lowQueue = queue;
}

void
PriorityQueueModel::SetHighQueue (Ptr<Queue> queue)
{
  NS_LOG_FUNCTION (this << queue);
  m_highQueue = queue;
}

bool
PriorityQueueModel::GetProcessNext (void)
{
  NS_LOG_FUNCTION_NOARGS();
  if (m_processNext)
    {
      return true;
    }
  else
    {
      m_processNext = true;
      return false;
    }
}

void
PriorityQueueModel::SetProcessNext (bool processNext)
{
  m_processNext = processNext;
}

uint16_t
PriorityQueueModel::Classify (Ptr<Packet> p)
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
      p->RemoveHeader (udp);

      if (udp.GetDestinationPort() == 3000)
        {
          NS_LOG_INFO ("\tclassifier: high priority udp");
          //NS_LOG_UNCOND ("\tclassifier: high priority udp " << p->GetUid());
          priority = 1;
        }
      else if (udp.GetDestinationPort() == 2000)
        {
          NS_LOG_INFO ("\tclassifier: low priority udp");
          priority = 0;
        }
      else
        {
          NS_LOG_INFO ("\tclassifier: unrecognized udp");
          NS_LOG_INFO ("\tclassifier: port=" << udp.GetDestinationPort());
          priority = 0;
        }
      p->AddHeader (udp);
    }
  else if (protocol == 6)
    {
      NS_LOG_INFO ("\tclassifier: tcp");
      priority = 2;
    }
  else
    {
      NS_LOG_INFO ("\tclassifier: unrecognized protocol");
      priority = 0;
    }

  p->AddHeader (ip);
  p->AddHeader (ppp);

  return priority;
}

uint32_t
PriorityQueueModel::Process (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this << packet);
  NS_LOG_INFO ("PriorityQueue: Received packet, UID = " << packet->GetUid());
  uint16_t priority = Classify (packet);

  uint32_t status;
  if (priority == 0)
    {
      if (m_isProcessing)
        {
          NS_LOG_INFO ("\tQueueing packet");
          if (m_lowQueue->Enqueue (packet))
            {
              status = 1;
            }
          else
            {
              //NS_LOG_INFO ("\tlowQueue overflow!");
              //NS_LOG_UNCOND ("\tlowQueue overflow!");
              status = 2;
            }
        }
      else
        {
          NS_LOG_INFO ("\tProcessing packet");
          m_isProcessing = true;
          status = 0;
        }
    }
  else if (priority == 1)
    {
      if (m_isProcessing)
        {
          NS_LOG_INFO ("\tQueueing packet");
          if (m_highQueue->Enqueue (packet))
            {
              status = 1;
            }
          else
            {
              //NS_LOG_INFO ("\thighQueue overflow!");
              //NS_LOG_UNCOND ("\thighQueue overflow!");
              status = 2;
            }
        }
      else
        {
          NS_LOG_INFO ("\tProcessing packet");
          m_isProcessing = true;
          status = 0;
        }
    }
  else if (priority == 2)
    {
      NS_LOG_INFO ("\tSending packet");
      status = 3;
    }
  return status;
}

Ptr<Packet>
PriorityQueueModel::ProcessComplete (uint64_t uid)
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_LOG_INFO ("PriorityQueue: Finished processing packet, UID = " << uid);
  // Choose next Packet
  Ptr<Packet> next;
  if (next = m_highQueue->Dequeue())
    {
      NS_LOG_INFO ("\tProcessing high priority packet, UID = " << next->GetUid());
    }
  else if (next = m_lowQueue->Dequeue())  
    {
      NS_LOG_INFO ("\tProcessing low priority packet, UID = " << next->GetUid());
    }
  else
    {
      NS_LOG_INFO ("\tNo more packets in queue");
      m_isProcessing = false;
    }

  m_processNext = false;
  return next;
}

/* Coordinated */
void
PriorityQueueModel::EnableThreshold (void)
{
  m_enableThreshold = true;
}

void
PriorityQueueModel::SetTxQueue (Ptr<Queue> queue)
{
  NS_LOG_FUNCTION (this << queue);
  m_txQueue = queue;
}

void
PriorityQueueModel::SetQueueMode (uint8_t mode)
{
  m_queueMode = mode;
}

void 
PriorityQueueModel::SetThresholdBytes (uint32_t threshold)
{
  m_thresholdBytes = threshold;
}

uint32_t 
PriorityQueueModel::GetThresholdBytes (void)
{
  return m_thresholdBytes;
}

void 
PriorityQueueModel::SetThresholdPackets (uint32_t threshold)
{
  m_thresholdPackets = threshold;
}

uint32_t 
PriorityQueueModel::GetThresholdPackets (void)
{
  return m_thresholdPackets;
}

bool 
PriorityQueueModel::CheckThreshold (uint32_t size)
{
  if (m_enableThreshold && m_txQueue)
    {
      if (m_queueMode == 'p')
        {
          NS_LOG_UNCOND("TxQueue: " << m_txQueue->GetNPackets() << " / " << m_thresholdPackets << " packets");
          NS_LOG_INFO("TxQueue: " << m_txQueue->GetNPackets() << " / " << m_thresholdPackets << " packets");
          //NS_LOG_UNCOND("total packets: " << m_txQueue->GetTotalReceivedPackets());
          if (m_txQueue->GetNPackets() + 1 >= m_thresholdPackets)
            {
              NS_LOG_INFO ("\tTxQueue is filled past threshold! Waiting...");
              NS_LOG_UNCOND ("\tTxQueue is filled past threshold! Waiting...");
              return false;
            }
        }
      else if (m_queueMode == 'b')
        {
          NS_LOG_UNCOND("TxQueue: " << m_txQueue->GetNBytes() << " / " << m_thresholdBytes << " bytes");
          NS_LOG_INFO("TxQueue: " << m_txQueue->GetNBytes() << " / " << m_thresholdBytes << " bytes");
          if (m_txQueue->GetNBytes() + size >= m_thresholdBytes) 
            {
              NS_LOG_INFO ("\tTxQueue is filled past threshold! Waiting...");
              //NS_LOG_UNCOND ("\tTxQueue is filled past threshold! Waiting...");
              return false;
            }
        }
    }
  return true;
}
/* Coordinated */

} // namespace ns3
