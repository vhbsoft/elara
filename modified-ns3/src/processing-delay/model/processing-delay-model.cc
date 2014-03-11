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
     m_constProcessingDelay(Seconds (0))
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

} // namespace ns3

