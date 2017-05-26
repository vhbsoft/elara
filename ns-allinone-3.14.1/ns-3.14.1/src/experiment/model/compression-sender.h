#ifndef COMPRESSION_SENDER_H
#define COMPRESSION_SENDER_H

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-address.h"

#include <string>

namespace ns3 {

class Socket;
class Packet;
class CompressionPacketGenerator;

/**
 * \sends probe packets to CompressionReceiver and analyzes the results it recieves
 */
class CompressionSender : public Application
{
public:
  static TypeId GetTypeId (void);
  CompressionSender ();
  virtual ~CompressionSender ();

  /**
   * \brief set the remote address and port
   * \param ip remote IP address
   * \param port remote port
   */
  void SetRemote (Ipv4Address ip, uint16_t udpPort, uint16_t tcpPort);
  void SetRemote (Address ip, uint16_t udpPort, uint16_t tcpPort);

  // Set the number of packets, inter packet departure spacing, packet length, entropy, and filename
  void SetNumPackets (uint32_t count);
  void SetInterval (Time interval);
  void SetPacketLen (uint32_t size);
  void SetEntropy (uint8_t entropy);
  void SetLogFileName (std::string name);
  void SetMode (uint8_t mode);
  // void SetIncludeTs (uint8_t includeTs);

protected:
  virtual void DoDispose (void);

private:

//
// Private Member Functions
//

  // If mode 0, sends request for TCP connection
  // If mode 1, schedule SendICMP()
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

  uint8_t m_initialized;
  
  // Maximum number of probe packets in the train
  uint32_t m_count;

  // Time interval between probe packets in the train
  Time m_interval;

  // Size of probe packets
  uint32_t m_size;

  // Entropy of probe packet data
  uint8_t m_entropy;  

  // Name of the output file
  std::string m_name;

  // Experiment mode, 0 - compliant receiver, 1 - non-compliant receiver
  uint8_t m_mode;

  // Tracks the number of probe packets already sent
  uint32_t m_sent;
  
  // Pointer to the socket to send probe packets to
  Ptr<Socket> m_udpSocket;

  // Pointer to the socket for TCP connections to Receiver
  Ptr<Socket> m_tcpSocket;

  // Address of CompressionReceiver
  Address m_peerAddress;

  // Port number for probe packets
  uint16_t m_peerUdpPort;

  // Port number for TCP connections
  uint16_t m_peerTcpPort;
 
  // Next scheduled event
  EventId m_nextEvent;

  // Generates packets filled with compressable data
  CompressionPacketGenerator m_gen;

}; // CompressionSender
  
  
} // namespace ns3

#endif /* COMPRESSION_SENDER_H */
