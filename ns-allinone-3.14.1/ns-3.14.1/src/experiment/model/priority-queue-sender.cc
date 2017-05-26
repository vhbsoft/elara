/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>

#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/tcp-socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/pointer.h"
#include "ns3/data-rate.h"

#include "ns3/exp-data-header.h"
#include "ns3/seq16-header.h"
#include "ns3/seq32-header.h"

#include "priority-queue-sender.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("PriorityQueueSender");
NS_OBJECT_ENSURE_REGISTERED (PriorityQueueSender);

TypeId
PriorityQueueSender::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::PriorityQueueSender")
    .SetParent<Application> ()
    .AddConstructor<PriorityQueueSender> ()
    .AddAttribute ("InitialPacketTrainLength",
                   "Number of non-apt priority probe packets initially sent to saturate the queue",
                   UintegerValue (1000),
                   MakeUintegerAccessor (&PriorityQueueSender::m_initialPacketTrainLength),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("SeparationPacketTrainLength",
                   "Number of packets between the apt priority probe packets",
                   UintegerValue (1000),
                   MakeUintegerAccessor (&PriorityQueueSender::m_separationPacketTrainLength),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("NumAptPriorityProbes",
                   "Number of apt priority probe packets sent",
                   UintegerValue (100),
                   MakeUintegerAccessor (&PriorityQueueSender::m_numAptPriorityProbes),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("AptPriority",
                   "Apt priority for this experiment", 
                   UintegerValue ('H'),
                   MakeUintegerAccessor (&PriorityQueueSender::m_aptPriority),
                   MakeUintegerChecker<uint8_t> ())
    .AddAttribute ("Interval",
                   "The time to wait between probe packets in the train", 
		   TimeValue (Seconds (1.0)),
                   MakeTimeAccessor (&PriorityQueueSender::m_interval),
                   MakeTimeChecker ())
    .AddAttribute ("PacketSize",
                   "Size of packets generated. The minimum packet size is 2 for just the sequence header",
                   UintegerValue (1024),
                   MakeUintegerAccessor (&PriorityQueueSender::m_size),
                   MakeUintegerChecker<uint32_t> (2, 10000))
    .AddAttribute ("RemoteAddress",
                   "The Address of the PriorityQueueReceiver",
                   AddressValue (),
                   MakeAddressAccessor (&PriorityQueueSender::m_peerAddress),
                   MakeAddressChecker ())
    .AddAttribute ("RemoteTcpPort", "The destination port of the outbound TCP connections",
                   UintegerValue (10),
                   MakeUintegerAccessor (&PriorityQueueSender::m_peerTcpPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("RemoteUdpPortHigh", "The destination port of the outbound high prope packets",
                   UintegerValue (3000),
                   MakeUintegerAccessor (&PriorityQueueSender::m_peerUdpPortHigh),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("RemoteUdpPortLow", "The destination port of the outbound low priority probe packets",
                   UintegerValue (2000),
                   MakeUintegerAccessor (&PriorityQueueSender::m_peerUdpPortLow),
                   MakeUintegerChecker<uint16_t> ())
  ;
  return tid;
}

PriorityQueueSender::PriorityQueueSender ()
{
  NS_LOG_FUNCTION_NOARGS ();
  m_initialized = 0;
  m_sent = 0;
  m_highSent = 0;
  m_lowSent = 0;
  m_initialSent = 0;
  m_aptSent = 0;
  m_separationSent = 0;
  m_udpSocketHigh = 0;
  m_udpSocketLow = 0;
  m_tcpSocket = 0;
  m_mode = 0;
  m_nextEvent = EventId ();
}

PriorityQueueSender::~PriorityQueueSender ()
{
  NS_LOG_FUNCTION_NOARGS ();
}


void
PriorityQueueSender::SetRemote (Ipv4Address ip, uint16_t udpPort,uint16_t tcpPort)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_peerAddress = Address(ip);
  m_peerTcpPort = tcpPort;
}

void
PriorityQueueSender::SetRemote (Address ip, uint16_t udpPort, uint16_t tcpPort)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_peerAddress = ip;
  m_peerTcpPort = tcpPort;
}

void 
PriorityQueueSender::SetAptPriority (uint8_t aptPriority)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_aptPriority = aptPriority;
}

void 
PriorityQueueSender::SetInitialPacketTrainLength (uint32_t initialPacketTrainLength)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_initialPacketTrainLength = initialPacketTrainLength;
}

void 
PriorityQueueSender::SetSeparationPacketTrainLength (uint32_t separationPacketTrainLength)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_separationPacketTrainLength = separationPacketTrainLength;
}

void 
PriorityQueueSender::SetNumAptPriorityProbes (uint32_t numAptPriorityProbes)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_numAptPriorityProbes = numAptPriorityProbes;
}

void 
PriorityQueueSender::SetInterval (Time interval)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_interval = interval;
}

void 
PriorityQueueSender::SetPacketLen (uint32_t size)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_size = size;
}

void
PriorityQueueSender::SetLogFileName (std::string name)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_name = name;
}

void
PriorityQueueSender::SetMode (uint8_t mode)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_mode = mode;
}

void
PriorityQueueSender::SetIntervalBackToBack (std::string txRateString)
{
  NS_LOG_FUNCTION_NOARGS ();
  
  double bits = (m_size + 30)*8; // network headers add 30 bytes
  double bps = (DataRate (txRateString)).GetBitRate();
  m_interval = Seconds (bits/bps);
  //NS_LOG_UNCOND("m_interval = " << m_interval);
}


void
PriorityQueueSender::DoDispose (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Application::DoDispose ();
}

void
PriorityQueueSender::StartApplication (void)
{

  NS_LOG_FUNCTION_NOARGS ();

  // Set up UDP high priority socket
  if (m_udpSocketHigh == 0)
    {
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_udpSocketHigh = Socket::CreateSocket (GetNode (), tid);
      if (Ipv4Address::IsMatchingType(m_peerAddress) == true)
        {
          m_udpSocketHigh->Bind ();
          m_udpSocketHigh->Connect (InetSocketAddress (Ipv4Address::ConvertFrom(m_peerAddress), m_peerUdpPortHigh));
        }
    }

  // Change to HandleUdpRead() once it has been implemented
  m_udpSocketHigh->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());

  // Set up UDP low priority socket
  if (m_udpSocketLow == 0)
    {
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_udpSocketLow = Socket::CreateSocket (GetNode (), tid);
      if (Ipv4Address::IsMatchingType(m_peerAddress) == true)
        {
          m_udpSocketLow->Bind ();
          m_udpSocketLow->Connect (InetSocketAddress (Ipv4Address::ConvertFrom(m_peerAddress), m_peerUdpPortLow));
        }
    }

  // Change to HandleUdpRead() once it has been implemented
  m_udpSocketLow->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());

  // Check mode
  if (m_mode == 0)
    {
      // Set up TCP socket
      if (m_tcpSocket == 0)
        {
          NS_LOG_INFO ("setting up tcp socket");

          TypeId tid = TypeId::LookupByName ("ns3::TcpSocketFactory");
          m_tcpSocket = Socket::CreateSocket (GetNode (), tid);
          m_tcpSocket->Bind ();
          m_tcpSocket->Connect (InetSocketAddress (Ipv4Address::ConvertFrom(m_peerAddress), m_peerTcpPort));
          m_tcpSocket->SetConnectCallback (
				           MakeCallback (&PriorityQueueSender::ConnectionSucceeded, this), // This is the next step
				           MakeCallback (&PriorityQueueSender::ConnectionFailed, this)
				          );
        }
      m_tcpSocket->SetCloseCallbacks (
				      MakeCallback (&PriorityQueueSender::HandleTcpClose, this),
				      MakeCallback (&PriorityQueueSender::HandleTcpError, this)
				     );
      
      m_tcpSocket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
    }

  else 
    {
      // UNFINISHED IMPLEMENTATION
      m_nextEvent = Simulator::Schedule (Seconds (0.0), &PriorityQueueSender::SendICMP, this);
    }
}

void
PriorityQueueSender::StopApplication ()
{
  NS_LOG_FUNCTION_NOARGS ();
  Simulator::Cancel (m_nextEvent);
}

void
PriorityQueueSender::InitializeTCP (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_ASSERT (m_nextEvent.IsExpired ());

  m_initialized = 1;

  m_totalProbePackets = m_initialPacketTrainLength + m_numAptPriorityProbes 
                        + (m_numAptPriorityProbes - 1) * m_separationPacketTrainLength;

  ExpDataHeader expData;
  expData.SetNumPackets (m_totalProbePackets);
  expData.SetInterPacketTime (m_interval.GetTimeStep());
  expData.SetProbePacketLen (m_size);

  expData.SetInitialNum (m_initialPacketTrainLength);
  expData.SetSeparationNum (m_separationPacketTrainLength);
  expData.SetAptNum (m_numAptPriorityProbes);
  expData.SetAptPriority (m_aptPriority);

  Ptr<Packet> p = Create<Packet> ();
  p->AddHeader(expData);

  NS_LOG_INFO ("Sending initializing packet:" <<
	       "\n\tnumPackets = " << m_totalProbePackets <<
  	       "\n\tinterPacketTime = " << m_interval.GetTimeStep() <<
	       "\n\tprobePacketLen = " << m_size <<
               "\n\tinitialPacketTrainLength = " << m_initialPacketTrainLength <<
               "\n\tseparationPacketTrainLength = " << m_separationPacketTrainLength <<
               "\n\tnumAptPriorityProbes = " << m_numAptPriorityProbes <<
               "\n\taptPriority = " << m_aptPriority);

  m_tcpSocket->Send(p);

  m_tcpSocket->SetRecvCallback (MakeCallback (&PriorityQueueSender::StartTrain, this));
}

void
PriorityQueueSender::SendICMP (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  // Add ICMP stuff here

  if (m_initialized == 0)
    {
      m_nextEvent = Simulator::Schedule (Seconds (0.0), &PriorityQueueSender::Send, this);  
      m_initialized = 1;
    }
}

void
PriorityQueueSender::StartTrain (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_ASSERT (m_nextEvent.IsExpired ());

  m_tcpSocket->Close();

  m_initialSent = 0;
  m_separationSent = 0;
  m_aptSent = 0;

  m_nextEvent = Simulator::Schedule (Seconds (1.0), &PriorityQueueSender::Send, this);

}

void
PriorityQueueSender::Send (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_ASSERT (m_nextEvent.IsExpired ());

  // Convert m_peerAddress for logging
  std::stringstream peerAddressStringStream;
  if (Ipv4Address::IsMatchingType (m_peerAddress))
    {
      peerAddressStringStream << Ipv4Address::ConvertFrom (m_peerAddress);
    }
  
  // Choose the correct socket
  uint8_t packetPriority;
  //uint16_t seqNum;
  if (m_initialSent < m_initialPacketTrainLength)
    {
      packetPriority = 'H';
      //seqNum = m_initialSent++;
      m_initialSent++;
    }
  else if (m_separationSent < m_separationPacketTrainLength && m_aptSent != 0)
    {
      if (m_aptPriority == 'H') packetPriority = 'L';
      else packetPriority = 'H';
      //seqNum = m_separationSent++;
      m_separationSent++;
    }
  else if (m_aptSent < m_numAptPriorityProbes)
    {
      packetPriority = m_aptPriority;
      m_separationSent = 0;
      //seqNum = m_aptSent++;
      m_aptSent++;
    }
  else
    {
      NS_LOG_ERROR ("Send did not return as expected");
      return;
    }

  // Create packet
  uint32_t size = m_size;
  Ptr<Packet> p;

  // Add priority specific sequence number and send out the correct socket
  uint32_t bytesSent;
  if (packetPriority == 'H')
    {
      // Add sequence/timestamp
      if (size >= 4)
        {
          // Leave 4 bytes for the Seq32Header
          p = Create<Packet>(size - 4);
          Seq32Header seq;
          seq.SetSeq (m_highSent);
          p->AddHeader (seq);
        }
      else
        {
          // Leave 2 bytes for the Seq16Header
          p = Create<Packet>(size - 2);
          Seq16Header seq;
          if (m_highSent > 65535){NS_LOG_ERROR("Seq16Header overflow!");exit(-1);}
          seq.SetSeq (m_highSent);
          p->AddHeader (seq); 
        }
      m_highSent++;
      bytesSent = m_udpSocketHigh->Send (p->Copy());
    }  
  else if (packetPriority == 'L')
    {
      // Add sequence/timestamp
      if (size >= 4)
        {
          // Leave 4 bytes for the Seq32Header
          p = Create<Packet>(size - 4);
          Seq32Header seq;
          seq.SetSeq (m_lowSent);
          p->AddHeader (seq);
        }
      else
        {
          // Leave 2 bytes for the Seq16Header
          p = Create<Packet>(size - 2);
          Seq16Header seq;
          if (m_lowSent > 65535){NS_LOG_ERROR("SeqHeader (16-bit) overflow!");exit(-1);}
          seq.SetSeq (m_lowSent);
          p->AddHeader (seq);
        }
      m_lowSent++;
      bytesSent = m_udpSocketLow->Send (p->Copy());
    }

  // Check if send was successfull
  if (bytesSent >= 0)
    {
      if (m_sent < 10)
        {
          NS_LOG_INFO("sent 00" << m_sent << '\t' << Simulator::Now().GetTimeStep());
        }
      else if (m_sent < 100)
        {
	  NS_LOG_INFO("sent 0" << m_sent << '\t' << Simulator::Now().GetTimeStep());
        }      
      else
        {
	  NS_LOG_INFO("sent " << m_sent << '\t' << Simulator::Now().GetTimeStep());
        }
      m_sent++;

      //NS_LOG_UNCOND ("bytesSent=" << bytesSent);
      //NS_LOG_UNCOND("sender::send "<<p->GetUid()<<" at "<<Simulator::Now().GetTimeStep());
      /*
      NS_LOG_INFO ("TraceDelay TX " << size << " bytes to "
		   << peerAddressStringStream.str () << " Uid: "
		   << p->GetUid () << " Time: "
		   << (Simulator::Now ()).GetSeconds ()
		   );
      */
    }
  else
    {
      NS_LOG_ERROR ("Error while sending " << size << " bytes to " << peerAddressStringStream.str ());
    }

  if (m_sent < m_totalProbePackets)
    {
      m_nextEvent = Simulator::Schedule (m_interval, &PriorityQueueSender::Send, this);
    }
  else if (m_sent == m_totalProbePackets)
    {
      return;
    }
  else
    {
      NS_LOG_ERROR ("Error m_sent (" << m_sent <<") > m_totalProbePackets (" << m_totalProbePackets << ")!");
      return;
    }
}

void
PriorityQueueSender::RequestData (void)
{
  NS_LOG_FUNCTION_NOARGS ();  

  TypeId tid = TypeId::LookupByName ("ns3::TcpSocketFactory");
  m_tcpSocket = Socket::CreateSocket (GetNode (), tid);
  m_tcpSocket->Bind ();
  m_tcpSocket->Connect (InetSocketAddress (Ipv4Address::ConvertFrom(m_peerAddress), 
			m_peerTcpPort)
			);

  m_tcpSocket->SetConnectCallback (
				   MakeCallback (&PriorityQueueSender::ConnectionSucceeded, this), // This is the next step 
				   MakeCallback (&PriorityQueueSender::ConnectionFailed, this)
				   );

  m_tcpSocket->SetCloseCallbacks (
    MakeCallback (&PriorityQueueSender::HandleTcpClose, this),
    MakeCallback (&PriorityQueueSender::HandleTcpError, this)
				  );

  m_tcpSocket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());

}

// SENDING DATA OVER TCP IS BROKEN
void
PriorityQueueSender::ReceiveData (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  Ptr<Packet> packet;
  Address from;
  packet = socket->RecvFrom (from);
  if (packet->GetSize() == 0)
    return;

  NS_LOG_INFO ("Receiving results: " << packet->GetSize() << " bytes" << 
	       " Uid: " << packet->GetUid () << " Time: " << (Simulator::Now ()).GetSeconds ()
               );

  ExpDataHeader expData;
  NS_LOG_INFO ("calling RemoveHeader");
  packet->RemoveHeader(expData);

  NS_LOG_INFO ("calling GetResults");
  std::string results = expData.GetResults();
  NS_LOG_INFO (results);

  std::ofstream output;
  output.open(m_name.c_str());
  output << results;
  output.close();
}

void
PriorityQueueSender::HandleUdpRead (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION_NOARGS ();
  // TO BE IMPLEMENTED
  // to get the ping backs in icmp mode
}

void 
PriorityQueueSender::ConnectionSucceeded (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  NS_LOG_INFO ("TCP Connection succeeded");

  if (m_initialized == 0)
    {
      m_nextEvent = Simulator::Schedule (Seconds (1.0), &PriorityQueueSender::InitializeTCP, this);
    }
  else
    {
      socket->SetRecvCallback (MakeCallback (&PriorityQueueSender::ReceiveData, this));
    }
}

void 
PriorityQueueSender::ConnectionFailed (Ptr<Socket> socket)
{
  NS_LOG_UNCOND ("TCP Connection Failed");

  NS_LOG_FUNCTION (this << socket);
  NS_LOG_INFO ("TCP Connection Failed");
  NS_LOG_ERROR ("TCP Connection Failed");
}

void 
PriorityQueueSender::HandleTcpClose (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
}
 
void 
PriorityQueueSender::HandleTcpError (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
}

} // Namespace ns3
