/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef PROCESSING_DELAY_MODEL_H
#define PROCESSING_DELAY_MODEL_H

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/nstime.h"
#include "ns3/queue.h"
#include "ns3/point-to-point-net-device.h"

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
  Time GetConstProcessingTime (void);

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

  void SetQueueMode (uint8_t mode);

  /* Coordinated */
  void EnableThreshold(void);
  void SetTxQueue (Ptr<Queue> queue);

  void SetThresholdBytes (uint32_t threshold);
  uint32_t GetThresholdBytes (void);
  void SetThresholdPackets (uint32_t threshold);
  uint32_t GetThresholdPackets (void);

  bool CheckThreshold (uint32_t size);
  /* Coordinated */  

private:
  
  bool m_isEnabled;
  bool m_isProcessing;

  bool m_processNext;
  Time m_constProcessingDelay;

  uint8_t m_queueMode;

  /* Coordinated */
  bool m_enableThreshold;
  uint32_t m_thresholdBytes;
  uint32_t m_thresholdPackets;
  Ptr<Queue> m_txQueue;
  /* Coordinated */
 
}; // ProcessingDelayModel

}  // namespace ns3

#endif /* PROCESSING_DELAY_MODEL_H */

