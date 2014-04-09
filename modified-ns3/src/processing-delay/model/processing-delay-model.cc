/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "processing-delay-model.h"

#include "ns3/log.h"
#include "ns3/boolean.h"

NS_LOG_COMPONENT_DEFINE ("ProcessingDelayModel");

namespace ns3 {
  
NS_OBJECT_ENSURE_REGISTERED (ProcessingDelayModel);
  
TypeId 
ProcessingDelayModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ProcessingDelayModel")
    .SetParent<Object> ()
    .AddConstructor<ProcessingDelayModel> ()
    .AddAttribute ("IsEnabled", "Whether or not this ProcessingDelayModel is enabled.",
		   BooleanValue (true),
		   MakeBooleanAccessor (&ProcessingDelayModel::m_isEnabled),
		   MakeBooleanChecker ())
    .AddAttribute ("IsProcessing", "Whether or not this ProcessingDelayModel is currently processing a packet.",
		   BooleanValue (false),
		   MakeBooleanAccessor (&ProcessingDelayModel::m_isProcessing),
		   MakeBooleanChecker ())
    .AddAttribute ("ProcessNext", "Whether or not to the next packet needs to be processed.",
		   BooleanValue (true),
		   MakeBooleanAccessor (&ProcessingDelayModel::m_processNext),
		   MakeBooleanChecker ())
    .AddAttribute ("ConstProcessingDelay", "The constant time it takes to process a packet.",
		   TimeValue (Seconds (0)),
		   MakeTimeAccessor (&ProcessingDelayModel::m_constProcessingDelay),
		   MakeTimeChecker ())
    ;
  return tid;
}

ProcessingDelayModel::ProcessingDelayModel ()
  :  m_isEnabled (true),
     m_isProcessing (false),
     m_processNext (true),
     m_constProcessingDelay(Seconds (0)),
     /* Coordinated */
     m_enableThreshold (false),
     m_thresholdBytes (0),
     m_thresholdPackets (0),
     m_txQueue (0)
     /* Coordinated */
{
  NS_LOG_FUNCTION_NOARGS ();
}
  
ProcessingDelayModel::~ProcessingDelayModel () 
{
  NS_LOG_FUNCTION_NOARGS ();
}
  
void
ProcessingDelayModel::Enable (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_isEnabled = true;
}
  
void
ProcessingDelayModel::Disable (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_isEnabled = false;
}

bool
ProcessingDelayModel::IsEnabled (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_isEnabled;
}

void
ProcessingDelayModel::SetConstProcessingTime (Time t)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_constProcessingDelay = t;
}

Time 
ProcessingDelayModel::GetConstProcessingTime (void)
{
  return m_constProcessingDelay;
}
void
ProcessingDelayModel::SetIsProcessing (bool isProcessing)
{
  NS_LOG_FUNCTION (isProcessing);
  m_isProcessing = isProcessing;
}

bool
ProcessingDelayModel::GetIsProcessing (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_isProcessing;
}

bool
ProcessingDelayModel::ProcessNext (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  if (m_processNext)
    {
      return m_processNext;
    }
  else 
    {
      m_processNext = true;
      return false;        
    }
}

Time 
ProcessingDelayModel::GetProcessingTime (void)
{
  return m_constProcessingDelay;
}
  
void 
ProcessingDelayModel::ProcessComplete (void)
{
  m_processNext = false;
}
  
/* Coordinated */
void
ProcessingDelayModel::EnableThreshold (void)
{
  m_enableThreshold = true;
}

void
ProcessingDelayModel::SetTxQueue (Ptr<Queue> queue)
{
  NS_LOG_FUNCTION (this << queue);
  m_txQueue = queue;
}

void
ProcessingDelayModel::SetQueueMode (uint8_t mode)
{
  m_queueMode = mode;
}

void
ProcessingDelayModel::SetThresholdBytes (uint32_t threshold)
{
  m_thresholdBytes = threshold;
}

uint32_t
ProcessingDelayModel::GetThresholdBytes (void)
{
  return m_thresholdBytes;
}

void
ProcessingDelayModel::SetThresholdPackets (uint32_t threshold)
{
  m_thresholdPackets = threshold;
}

uint32_t
ProcessingDelayModel::GetThresholdPackets (void)
{
  return m_thresholdPackets;
}

bool
ProcessingDelayModel::CheckThreshold (uint32_t size)
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
