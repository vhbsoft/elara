/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef PROCESSING_DELAY_RECEIVER_H
#define PROCESSING_DELAY_RECEIVER_H

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/address.h"
#include "ns3/socket.h"
#include <string>

namespace ns3 {

class ProcessingDelayReceiver : public Application
{
public:
  static TypeId GetTypeId (void);
  ProcessingDelayReceiver ();
  virtual ~ProcessingDelayReceiver ();

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
  
  // Receive probe packets and log their arrival time
  void HandleUdpRead (Ptr<Socket> socket);

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

  // 0 for tcp, 1 for icmp
  // ICMP NOT YET IMPLEMENTED
  uint8_t m_mode;

  // Local address and port info
  Address m_local;
  uint16_t m_udpPort;
  uint16_t m_tcpPort;
  Ptr<Socket> m_udpSocket;
  Ptr<Socket> m_tcpSocket;
  Ptr<Socket> m_connection;
 
  // Address and port info for Sender
  Address m_peerAddress;
  uint16_t m_peerUdpPort;
  uint16_t m_peerTcpPort;

  // Experiment variables
  uint32_t m_received;
  uint8_t m_initialized;
  EventId m_nextEvent;

  uint32_t m_numPackets;
  uint64_t m_interPacketTime;
  uint32_t m_probePacketLen;
  int64_t* m_results;
};

} // namespace ns3

#endif /* PROCESSING_DELAY_RECEIVER_H */
