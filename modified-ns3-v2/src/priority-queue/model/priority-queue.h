/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <queue>
//#include "ns3/ptr.h"
#include "ns3/packet.h"
#include "ns3/queue.h"

namespace ns3 {

  class TraceContainer;

class PriorityQueue : public Queue {
public:
  static TypeId GetTypeId (void);

  PriorityQueue ();

  ~PriorityQueue ();

  void SetMode (PriorityQueue::QueueMode mode);
  PriorityQueue::QueueMode GetMode (void);

private:

  virtual bool DoEnqueue (Ptr<Packet> p);
  virtual Ptr<Packet> DoDequeue (void);
  virtual Ptr<const Packet> DoPeek (void) const;

  // returns 0 for low
  // returns 1 for high
  uint16_t Classify (Ptr<Packet> p);

  std::queue<Ptr<Packet> > m_highPackets;
  std::queue<Ptr<Packet> > m_lowPackets;
  uint32_t m_maxPackets;
  uint32_t m_maxBytes;
  uint32_t m_bytesInHighQueue;
  uint32_t m_bytesInLowQueue;
  QueueMode m_mode;

};

} // namespace ns3

#endif /* PRIORITY_QUEUE_H */

