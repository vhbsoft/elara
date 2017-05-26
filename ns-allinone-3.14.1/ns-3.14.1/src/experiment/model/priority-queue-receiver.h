/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef PRIORITY_QUEUE_RECEIVER_H
#define PRIORITY_QUEUE_RECEIVER_H

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/address.h"
#include "ns3/socket.h"
#include <string>

namespace ns3 {

class PriorityQueueReceiver : public Application
{
public:
  static TypeId GetTypeId (void);
  PriorityQueueReceiver ();
  virtual ~PriorityQueueReceiver ();

  void SetMode (uint8_t mode);
  void SetLogFileName (std::string name);

  // After Sender reestablishes TCP connection, stop listening for packets, 
  // reformat arrival data and return to Sender
  void Process (void);

protected:
  virtual void DoDispose (void);

private:

  virtual void StartApplication (void);
  virtual void StopApplication (void);
  
  // Receive high priority probe packets and log their arrival time
  void HandleUdpReadHigh (Ptr<Socket> socket);

  // Receive low priority probe packets and log their arrival time
  void HandleUdpReadLow (Ptr<Socket> socket);

  // Handle Tcp requests from Sender
  void Initialize (Ptr<Socket> socket);

  //
  void HandleTcpAccept (Ptr<Socket> socket, const Address& from);

  //
  void HandleTcpClose (Ptr<Socket> socket);

  //
  void HandleTcpError (Ptr<Socket> socket);

//
// Private Member Variables
//

  // Name of the output file                                                                             
  std::string m_name;

  // 0 for tcp
  uint8_t m_mode;

  // Local address and port info
  Address m_local;
  uint16_t m_udpPortHigh;
  uint16_t m_udpPortLow;
  uint16_t m_tcpPort;
  Ptr<Socket> m_udpSocketHigh;
  Ptr<Socket> m_udpSocketLow;
  Ptr<Socket> m_tcpSocket;
  Ptr<Socket> m_connection;
 
  // Address and port info for Sender
  Address m_peerAddress;
  uint16_t m_peerUdpPortHigh;
  uint16_t m_peerUdpPortLow;
  uint16_t m_peerTcpPort;

  // Experiment variables
  uint32_t m_received;
  uint8_t m_initialized;
  EventId m_nextEvent;

  uint32_t m_numPackets;
  uint64_t m_interPacketTime;
  uint32_t m_probePacketLen;

  uint32_t m_initialNum;
  uint32_t m_separationNum;
  uint32_t m_aptNum;
  uint8_t m_aptPriority;

  uint32_t m_numLowPriority;
  uint32_t m_numHighPriority;

  uint32_t m_lastHighSeq;
  uint32_t m_lastLowSeq;
  int64_t m_lastHighTs;
  int64_t m_lastLowTs;

  int32_t m_stage;

  int64_t *m_resultsLow;
  int64_t *m_resultsHigh;
  //int64_t *m_resultsBoth;
  //uint8_t *m_resultsBothType;
};

} // namespace ns3

#endif /* PRIORITY_QUEUE_RECEIVER_H */

