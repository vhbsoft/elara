/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef PROCESSING_DELAY_MODEL_H
#define PROCESSING_DELAY_MODEL_H

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/nstime.h"

namespace ns3 {

class Packet;

class ProcessingDelayModel : public Object
{
public:
  static TypeId GetTypeId (void);

  ProcessingDelayModel ();
  ~ProcessingDelayModel ();

  void Enable (void);
  void Disable (void);
  bool IsEnabled (void) const;

  void SetConstProcessingTime (Time t);

  void SetIsProcessing (bool isProcessing);
  bool GetIsProcessing (void) const;

  // Called to check whether or not to process the next packet
  // - If m_processNext is true return true
  // - If m_processNext is false, set it to true and return false
  bool ProcessNext (void);

  // Returns the amount of processing delay. 
  Time GetProcessingTime (void);

  // Called when a single packet has just finished processing
  // - Sets m_processNext to false
  void ProcessComplete (void);

private:
  
  bool m_isEnabled;
  bool m_isProcessing;

  bool m_processNext;
  Time m_constProcessingDelay;
 
}; // ProcessingDelayModel

}  // namespace ns3

#endif /* PROCESSING_DELAY_MODEL_H */

