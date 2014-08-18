/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef PRIORITY_QUEUE_SENDER_H
#define PRIORITY_QUEUE_SENDER_H

#include <string>

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-address.h"
#include "ns3/data-rate.h"

namespace ns3 {

class Socket;
class Packet;

/**
 * \sends probe packets to PriorityQueueReceiver and analyzes the results it recieves
 */
class PriorityQueueSender : public Application
{
public:
  static TypeId GetTypeId (void);
  PriorityQueueSender ();
  virtual ~PriorityQueueSender ();

  /**
   * \brief set the remote address and port
   * \param ip remote IP address
   * \param port remote port
   */
  void SetRemote (Ipv4Address ip, uint16_t udpPort, uint16_t tcpPort);
  void SetRemote (Address ip, uint16_t udpPort, uint16_t tcpPort);

  void SetAptPriority (uint8_t aptPriority);
  void SetInitialPacketTrainLength (uint32_t initialPacketTrainLength);
  void SetSeparationPacketTrainLength (uint32_t separationPacketTrainLength);
  void SetNumAptPriorityProbes (uint32_t numAptPriorityProbes);

  // Set the inter packet departure spacing, packet length, entropy, and filename  
  void SetInterval (Time interval);
  void SetPacketLen (uint32_t size);
  void SetLogFileName (std::string name);

  void SetIntervalBackToBack (std::string txRateString);

  // unused
  void SetMode (uint8_t mode);

protected:
  virtual void DoDispose (void);

private:

//
// Private Member Functions
//

  // If mode 0, sends request for TCP connection
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  // Send initialization data over TCP connection,
  // schedules StartTrain() after socket callback
  void InitializeTCP (void);

  // Send a ping request to receiver
  // If first ping, schedule StartTrain()
  // If second ping, do nothing
  void SendICMP (void);  

  // mode 0: Tear down TCP connection
  // schedule first Send()
  void StartTrain (Ptr<Socket> socket);

  // Send probe packet and schedule next Send()
  // mode 0: If all probe packets in the train have been sent schedule TcpRequest()
  void Send (void);  

  void RequestData (void);

  // Receive experiment data from Receiver over TCP connection,
  // save results to file
  // SENDING DATA OVER TCP IS BROKEN
  void ReceiveData (Ptr<Socket> socket);

  // UDP callback responses
  void HandleUdpRead (Ptr<Socket> socket);

  // TCP callback responses
  // ConnectionSucceeded() is used to schedule primary functions 
  void ConnectionSucceeded (Ptr<Socket> socket);
  void ConnectionFailed (Ptr<Socket> socket);
  void HandleTcpClose (Ptr<Socket> socket);
  void HandleTcpError (Ptr<Socket> socket);

//
// Private Member Variables
//

  // Number of non-apt priority probe packets initially sent to saturate the queue
  uint32_t m_initialPacketTrainLength;
  
  // Number of packets between the apt priority probe packets
  uint32_t m_separationPacketTrainLength;

  // Number of apt priority probe packets sent
  uint32_t m_numAptPriorityProbes;
 
  // Apt priority for this experiment
  uint8_t m_aptPriority;

  // Time interval between probe packets in the train
  Time m_interval;

  // Size of probe packets in bytes
  uint32_t m_size;

  // Name of the output file
  std::string m_name;

  // Experiment mode, 0 - compliant receiver
  uint8_t m_mode;

  uint8_t m_initialized;

  // Total number of probe packets for this experiment
  uint32_t m_totalProbePackets;

  // Tracks the number of probe packets already sent
  uint32_t m_sent;

  // Tracks the number of high priority packets already sent
  uint32_t m_highSent;

  // Tracks the number of low priority packets already sent
  uint32_t m_lowSent;

  // Tracks the number of initial packets already sent
  uint32_t m_initialSent;
  
  // Tracks the number of apt priority packets already sent
  uint32_t m_aptSent;

  // Tracks the number of separation packets already sent since last apt packet
  uint32_t m_separationSent;

  // Pointer to the socket to send high priority probe packets to
  Ptr<Socket> m_udpSocketHigh;

  // Pointer to the socket to send low priority probe packets to
  Ptr<Socket> m_udpSocketLow;

  // Pointer to the socket for TCP connections to Receiver
  Ptr<Socket> m_tcpSocket;

  // Address of PriorityQueueReceiver
  Address m_peerAddress;

  // Port number for high priority probe packets
  uint16_t m_peerUdpPortHigh;

  // Port number for low priority probe packets
  uint16_t m_peerUdpPortLow;

  // Port number for TCP connections
  uint16_t m_peerTcpPort;
 
  // Next scheduled event
  EventId m_nextEvent;

}; // PriorityQueueSender
  
  
} // namespace ns3

#endif /* PRIORITY_QUEUE_SENDER_H */

