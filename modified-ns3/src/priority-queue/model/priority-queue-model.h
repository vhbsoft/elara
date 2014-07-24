/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef PRIORITY_QUEUE_MODEL_H
#define PRIORITY_QUEUE_MODEL_H

#include "ns3/object.h"
#include "ns3/nstime.h"
#include "ns3/ptr.h"
#include "ns3/queue.h"
#include "ns3/point-to-point-net-device.h"

namespace ns3 {

class Packet;

class PriorityQueueModel : public Object
{
public:
  static TypeId GetTypeId (void);

  PriorityQueueModel ();
  ~PriorityQueueModel ();

  void Enable (void);
  void Disable (void);
  bool IsEnabled (void) const;

  void SetNodalProcessingTime (Time t);
  Time GetNodalProcessingTime (void) const;

  void SetLowQueue (Ptr<Queue> queue);  

  void SetHighQueue (Ptr<Queue> queue);  

  // Called by PointToPointNetDevice
  void SetNetDevice (Ptr<PointToPointNetDevice> netDevice);

  bool GetProcessNext (void);
  void SetProcessNext (bool processNext);

  // returns 0 if packet is immediately processing
  // returns 1 if successfully queued
  // returns 2 if the packet could not be queued
  uint32_t Process (Ptr<Packet> packet);

  Ptr<Packet> ProcessComplete (uint64_t uid);

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

  // returns 0 for low
  // returns 1 for high
  // returns 2 for super-high (skips queue)
  uint16_t Classify (Ptr<Packet> p);
  /* void ProcessComplete (Ptr<Packet> p);*/

  Ptr<PointToPointNetDevice> m_netDevice;

  bool m_isEnabled;
  bool m_isProcessing;

  bool m_processNext;
  Time m_nodalProcessingDelay;

  Ptr<Queue> m_highQueue;
  Ptr<Queue> m_lowQueue;

  uint8_t m_queueMode;

  /* Coordinated */
  bool m_enableThreshold;
  uint32_t m_thresholdBytes;
  uint32_t m_thresholdPackets;
  Ptr<Queue> m_txQueue;
  /* Coordinated */

};

} // namespace ns3

#endif /* PRIORITY_QUEUE_MODEL_H */

